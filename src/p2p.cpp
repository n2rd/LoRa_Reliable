#include "main.h"
#include "p2p.h"
#include <pthread.h>
#include "ArduinoQueue.h"
#include "reversePriorityQueue.h"

#ifndef USE_RANDOM_SIGREP_SLOT
#define USE_RANDOM_SIGREP_SLOT true
#endif

bool randomSignalReportSlot = USE_RANDOM_SIGREP_SLOT;

//
//Peer to Peer Messaging
//

//Packet Header Bits defined
#define PACKET_HEADER_BIT_8CHAR_MAIDENHEAD  0x00000001
#define PACKET_HEADER_BIT_NEED_TIME         0x00000010
#define PACKET_HEADER_BIT_HAVE_TIME         0x00000100

// Pause between transmited packets in seconds.
#define PAUSE       PARMS.parameters.tx_interval // client, time between transmissions in seconds

// some state variables
extern bool menu_active;
bool short_pause = false;
unsigned long effective_pause = 0;
uint64_t broadcast_time = 0;

// Dont put this on the stack: 
uint8_t buf[DRIVER_MAX_MESSAGE_LEN];

typedef struct {
  uint32_t transmitTime;
  uint8_t to;
  uint8_t from;
  uint8_t len;
  uint8_t headerID;
  uint8_t flags;
  uint8_t data[DRIVER_MAX_MESSAGE_LEN];
  char    gridLocator[11];
} transmitMessage_t;

typedef struct  {
  unsigned long timeStamp;
  uint8_t to;
  uint8_t from;
  uint8_t headerID;
  uint8_t id;
  uint8_t flags;
  uint8_t len;
  uint8_t packet[DRIVER_MAX_MESSAGE_LEN];
  int     snr;
  int     rssi;
} recvMessage_t;
/*
typedef struct TESTSTRUCT {
  unsigned long timeStamp;
  uint8_t to;
  uint8_t from;
  uint8_t headerID;
  uint8_t id;
  uint8_t flags;
  uint8_t len;
  //uint8_t packet[DRIVER_MAX_MESSAGE_LEN];
  char    letter;
  int     snr;
  char    letter2;
  int     rssi;
} testStruct_t;

typedef struct __attribute__((packed)) TESTPACKEDSTRUCT {
  unsigned long timeStamp;
  uint8_t to;
  uint8_t from;
  uint8_t headerID;
  uint8_t id;
  uint8_t flags;
  uint8_t len;
  //uint8_t packet[DRIVER_MAX_MESSAGE_LEN];
  char    letter;
  int     snr;
  char    letter2;
  int     rssi;
} testPackedStruct_t;
*/
#define MAX_QUEUE 50  //max queue size
ReversePriorityQueue<transmitMessage_t> transmit_queue(MAX_QUEUE);
ArduinoQueue<recvMessage_t> receive_queue(MAX_QUEUE);

uint8_t transmit_headerId = 0;

//keep track of the last time a message was transmitted
uint64_t tx_time = 0;

void extractGrid6LocatorFromData(int startMsgDataIndex, uint8_t* data, int dataLen, char* locator);

//--------------------------------------------------------------------------------------------------
TaskHandle_t p2pTaskHandle;

DECLARE_MUTEX(csvOutputMutex);
DECLARE_MUTEX(receivedQueueMutex);
DECLARE_MUTEX(transmitQueueMutex);
DECLARE_MUTEX(radioHeadMutex);
//--------------------------------------------------------------------------------------------------
bool checkReceiveQueueForItem(){
  MUTEX_LOCK(receivedQueueMutex);
  bool bRet = !receive_queue.isEmpty();
  MUTEX_UNLOCK(receivedQueueMutex);
  return bRet;
}
//--------------------------------------------------------------------------------------------------
bool checkTransmitQueueForItem(){
  MUTEX_LOCK(transmitQueueMutex);
  unsigned int count = transmit_queue.itemCount();
  if (count > (MAX_QUEUE / 2)) {
    char msg[] = "Transmit Queue has %4u items";
    char buf[sizeof(msg)+4];
    sprintf(buf,msg,count);
    debugMessage(buf);
  }
  bool bRet = !transmit_queue.isEmpty();
  MUTEX_UNLOCK(transmitQueueMutex);
  return bRet;
}
//--------------------------------------------------------------------------------------------------
void p2pTaskDisplayCSV(void *pvParameter)
{
  do {
    recvMessage_t receivedMsg;
    while (checkReceiveQueueForItem()) {
      int rssi;
      int snr;
      MUTEX_LOCK(receivedQueueMutex);
      receivedMsg = receive_queue.dequeue();
      MUTEX_UNLOCK(receivedQueueMutex);
      uint8_t to = receivedMsg.to;
      uint8_t from = receivedMsg.from;
      uint8_t headerId = receivedMsg.headerID;
      uint8_t len = receivedMsg.len;
      char gridLocator[11];
      char csvChar = 'X';
      extractGrid6LocatorFromData(2, receivedMsg.packet, len, gridLocator);
      gridLocator[6] = 0;
      if (to == RH_BROADCAST_ADDRESS) {
        //display the broadcast message
        snr = receivedMsg.snr;
        rssi = receivedMsg.rssi;
        if (!menu_active) {
          display.printf(
            "B %u-%03u #%i RSSI %i\n",
            from,
            headerId,
            (int)(receivedMsg.packet[0]*256 + receivedMsg.packet[1]),
            rssi
            ); //Do it all in one
        }
        csvChar = 'B';
      } else {
        //we have a signal report for us or another address (if in promiscuous mode)
        rssi = (int8_t)receivedMsg.packet[0];
        snr = receivedMsg.packet[1];

        if (to == PARMS.parameters.address) 
          csvChar = 'S';
        else
          csvChar = 'P';
        if (!menu_active) {
          display.printf("%c %u-%03u RSSI %i\n",csvChar, from, headerId, rssi);
        }
      }
      MUTEX_LOCK(csvOutputMutex);
      csv_serial.data(receivedMsg.timeStamp, csvChar, from, to, headerId, rssi, snr, gridLocator);
      csv_telnet.data(receivedMsg.timeStamp, csvChar, from, to, headerId, rssi, snr, gridLocator);
      MUTEX_UNLOCK(csvOutputMutex);
    }
    delay(25);
  } while(true);
}
//--------------------------------------------------------------------------------------------------
uint64_t getDeterministicSlot() //return a time for transmit based on our address
{
  //return ((esp_random() % 100 /*avg slots between broadcasts */) * 300 /*slot size in ms*/ ) + millis();
  int num_slots = ((PARMS.parameters.tx_interval *1000)/DETERMINISTIC_SIGREP_SLOT_WIDTH)-1;     //# of slots  is transmit interval in milliseconds / slot width in ms
  int radio_address_scale_factor = num_slots/DETERMINISTIC_SIGREP_MAX_RADIO_ADDRESS;
  return (PARMS.parameters.address * radio_address_scale_factor * DETERMINISTIC_SIGREP_SLOT_WIDTH) + millis();
  //return (PARMS.parameters.address * 3 * 300) + millis();
}
//--------------------------------------------------------------------------------------------------
uint64_t getRandomSlot()
{
  return ((esp_random() % 100 /*avg slots between broadcasts */) * 300 /*slot size in ms*/ ) + millis();
}
//--------------------------------------------------------------------------------------------------
#define GPS_FIX_TIMEOUT 10000

void addGrid6LocatorIntoMsg(transmitMessage_t* messagePtr, char **gridLocatorPtr = NULL)
{
  const int gridSize = 6;
  double lat = 0, lon = 0;
  bool hasFix;
  if (GPS.onoffState() == GPS.GPS_OFF) {
      log_d("GPS powered off - using fixed location");
      char* fixedMaidenheadGrid;
      fixedMaidenheadGrid = 
        GPS.latLonToMaidenhead(
          PARMS.parameters.lat_value,
          PARMS.parameters.lon_value,
          6
          );
      log_d("Fixed lat %lf, lon %lf, grid %s",PARMS.parameters.lat_value,PARMS.parameters.lon_value,fixedMaidenheadGrid);
      if (gridLocatorPtr != NULL)
        *gridLocatorPtr = fixedMaidenheadGrid;
      encode_grid4_to_buffer(fixedMaidenheadGrid,&messagePtr->data[messagePtr->len]);
      messagePtr->len+=2;
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[4];
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[5];
      strcpy(messagePtr->gridLocator, fixedMaidenheadGrid);
      return;
  }
  else {
    unsigned long beforeFix = millis();
    do {
      hasFix = !GPS.getLocation(&lat,&lon);
    } while(!hasFix || ((millis()-beforeFix) > GPS_FIX_TIMEOUT));
    if (!hasFix) {
      //We timed out
      log_d("Timeout getting GPS.getLocation() fix");
    }
    if (!hasFix || ((lat == 0.0) && (lon == 0.0))) {
      //Couldn't get a fix
      //log_d("GPS powered on but no fix in timeout- using fixed");
      //Debug code for baudrateswith
      if (!GPS.getRtcIsSet()) {
        log_d("Time not set by GPS or needs correcting diff: %d",GPS.getTimeDiff());
        if (GPS.getBaudTestBufferPtr())
          log_d("Baud: %d  Reason: %d BufLen %d Buffer: %s",GPS.getBaudRate(),GPS.baudSwitchReason,GPS.baudTestBufferLen,GPS.getBaudTestBufferPtr());
      }
      //Stuff fixed into ?? 
      char* fixedMaidenheadGrid;
      fixedMaidenheadGrid = 
        GPS.latLonToMaidenhead(
          PARMS.parameters.lat_value,
          PARMS.parameters.lon_value,
          6
          );
      if (gridLocatorPtr != NULL)
        *gridLocatorPtr = fixedMaidenheadGrid;

      //log_d("Fixed lat %lf, lon %lf, grid %s",PARMS.parameters.lat_value,PARMS.parameters.lon_value,fixedMaidenheadGrid);
      encode_grid4_to_buffer(fixedMaidenheadGrid,&messagePtr->data[messagePtr->len]);
      messagePtr->len+=2;
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[4];
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[5];
      strcpy(messagePtr->gridLocator, fixedMaidenheadGrid);
      return;
    }

    char *curMaidenheadGrid = GPS.latLonToMaidenhead(lat,lon, gridSize);
    //log_d("GPS lat %lf, lon %lf, grid %s",PARMS.parameters.lat_value,PARMS.parameters.lon_value,curMaidenheadGrid);
    if (gridLocatorPtr != NULL)
      *gridLocatorPtr = curMaidenheadGrid;
    encode_grid4_to_buffer(curMaidenheadGrid,&messagePtr->data[messagePtr->len]);
    messagePtr->len+=2;
    messagePtr->data[messagePtr->len++] = (uint8_t)curMaidenheadGrid[4];
    messagePtr->data[messagePtr->len++] = (uint8_t)curMaidenheadGrid[5];
    strcpy(messagePtr->gridLocator, curMaidenheadGrid);
  }
}
//--------------------------------------------------------------------------------------------------
void extractGrid6LocatorFromData(int startMsgDataIndex, uint8_t* data, int dataLen, char* locator)
{
  uint16_t mindex = startMsgDataIndex;
  uint16_t lindex = 4;
  if (dataLen >= (startMsgDataIndex+2)) {
    decode_grid4_from_buffer(&data[mindex],locator);
    if (dataLen >= (startMsgDataIndex+4)) {
      mindex+=2;
      locator[lindex++] = data[mindex++]; //5th locator character
      locator[lindex++] = data[mindex++]; //6th locator character
      locator[lindex] = 0;
    }
  } 
}
//--------------------------------------------------------------------------------------------------
void queueABroadcastMsg(uint8_t from = RH_BROADCAST_ADDRESS, unsigned long timeToSend = -1)
{
  //add the broadcast message to the message queue
  int8_t temp = bmp280_isPresent() ? (int8_t) myBMP280.readTempF() : 0xFF;
  transmitMessage_t message;
  message.data[0] = 0;  //static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
  message.data[1] = temp; //static_cast<uint8_t>(counter & 0xFF); //low byte
  message.len = 2;
  message.flags = 0; /* Placholder for flags */
  message.headerID = ++transmit_headerId;
  message.to = RH_BROADCAST_ADDRESS;
  if (from == RH_BROADCAST_ADDRESS)
    from = PARMS.parameters.address;
  message.from = from;
  if (timeToSend == -1)
    message.transmitTime = millis(); /* transmit now! */
  else
    message.transmitTime = timeToSend;
  //Add maidenheadGrid6
  char* gridLocator;
  addGrid6LocatorIntoMsg(&message);
  MUTEX_LOCK(transmitQueueMutex);
  if (!transmit_queue.isFull()) {
    transmit_queue.enqueue(message, message.transmitTime);
    MUTEX_UNLOCK(transmitQueueMutex);
  } else {
    MUTEX_UNLOCK(transmitQueueMutex);
    MUTEX_LOCK(csvOutputMutex);
    csv_serial.debug("p2p",(char *)"Transmit queue full\n");
    csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
    MUTEX_UNLOCK(csvOutputMutex);
  }
}
//--------------------------------------------------------------------------------------------------
void transmitAQueuedMsg()
{
  //transmit the top message in the queue at it's delay getRandom300msecSlot time
  if (checkTransmitQueueForItem()) {
    transmitMessage_t message, *messagePtr;
    MUTEX_LOCK(transmitQueueMutex);
    messagePtr = transmit_queue.getHeadPtr();
    uint32_t currentTime = millis();
    //log_d("Delay till %u millis. current millis = %u",messagePtr->transmitTime,millis());
    if (messagePtr->transmitTime <= currentTime) {
      message = transmit_queue.dequeue();
      MUTEX_UNLOCK(transmitQueueMutex);
      if (!PARMS.parameters.tx_lock) { //skip sending of the queued message.
        //unsigned long curMicros = micros();
        MUTEX_LOCK(radioHeadMutex);
        manager.setHeaderId(message.headerID);
        manager.setHeaderFrom(message.from);
        //log_d("Before sendto(..) rxBad %d rxGood %d txGood %d mode: %d",driver.rxBad(),driver.rxGood(),driver.txGood(), driver.mode());
        bool mgrRet = manager.sendto(message.data, message.len, message.to);
        MUTEX_UNLOCK(radioHeadMutex);
        if (!mgrRet) {
          MUTEX_LOCK(transmitQueueMutex);
          //requeue failed message
          transmit_queue.enqueue(message, message.transmitTime);
          MUTEX_UNLOCK(transmitQueueMutex);
          log_e("manager.sendto(..) failed message requeued");
        }
        MUTEX_LOCK(radioHeadMutex);
        while(driver.mode() == RHGenericDriver::RHModeTx) {
          MUTEX_UNLOCK(radioHeadMutex);
          delayMicroseconds(100);
          MUTEX_LOCK(radioHeadMutex);
        }
        //log_d("After sendto(..) rxBad %d rxGood %d txGood %d",driver.rxBad(),driver.rxGood(),driver.txGood());
        RHGenericDriver::RHMode curMode = driver.mode();
        if (driver.mode() == RHGenericDriver::RHModeIdle)
          driver.setModeRx();
        MUTEX_UNLOCK(radioHeadMutex);
        //unsigned long transmitMicros = micros() - curMicros;
        //log_d("Transmit time: %ld µs", transmitMicros);
        uint8_t from = manager.thisAddress();
        tx_time = millis();
        if (message.to == RH_BROADCAST_ADDRESS) {
          //TODO Queue the broadcast CSV ouput up
          MUTEX_LOCK(csvOutputMutex);
          csv_serial.broadcast(GPS.getTimeStamp(), from, message.headerID, message.gridLocator);
          csv_telnet.broadcast(GPS.getTimeStamp(), from, message.headerID, message.gridLocator);
          MUTEX_UNLOCK(csvOutputMutex);
        }
        else { // message is a signal report
          //TODO Queue the signal report CSV ouput up
          MUTEX_LOCK(csvOutputMutex);
          csv_serial.signalReport(GPS.getTimeStamp(), from, message.to, message.headerID, message.gridLocator);
          csv_telnet.signalReport(GPS.getTimeStamp(), from, message.to, message.headerID, message.gridLocator);
          MUTEX_UNLOCK(csvOutputMutex);
        }
      }
    }
    else {
      MUTEX_UNLOCK(transmitQueueMutex);
    }
  } //if it is time to transmit a message
}
//--------------------------------------------------------------------------------------------------
void listenForMessage()
{
  MUTEX_LOCK(radioHeadMutex);
  //log_d("top of listenFor Message mode: %d",(int)driver.mode());
  bool isAvailable = manager.available();
  MUTEX_UNLOCK(radioHeadMutex);
  while (isAvailable) { //message has come in
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;
    uint8_t id;
    uint8_t flags;
    MUTEX_LOCK(radioHeadMutex);
    if (manager.recvfrom(buf, &len, &from, &to, &id, &flags)) {
      uint8_t headerId = driver.headerId();
      recvMessage_t receivedMsg;
      receivedMsg.timeStamp = GPS.getTimeStamp();
      memcpy(receivedMsg.packet,buf,len);
      receivedMsg.len = len;
      receivedMsg.to = to;
      receivedMsg.from = from;
      receivedMsg.id = id;
      receivedMsg.flags = flags;
      /*{
        char buf[12];
        sprintf(buf,"flags = %2X",flags);
        debugMessage(buf);
      }*/
      receivedMsg.headerID = headerId;
      MUTEX_UNLOCK(radioHeadMutex);
      if (to == RH_BROADCAST_ADDRESS) {
        //we have a broadcast message, so send reply back to sender
        //and display the broadcast message
        int snr = driver.lastSNR();
        int rssi = driver.lastRssi();
        receivedMsg.snr = snr;
        receivedMsg.rssi = rssi;
        MUTEX_LOCK(receivedQueueMutex);
        if (!receive_queue.isFull()) {
          receive_queue.enqueue(receivedMsg); //Queue msg for display/csv output
          MUTEX_UNLOCK(receivedQueueMutex);
        }
        else {
          MUTEX_UNLOCK(receivedQueueMutex);
          MUTEX_LOCK(csvOutputMutex);
          csv_serial.debug("p2p",(char *)"receive_queue full\n");
          csv_telnet.debug("p2p",(char *)"receive_queue full\n");
          MUTEX_UNLOCK(csvOutputMutex);
        }

        //create and add a signal report to the message queue
        transmitMessage_t message;
        message.data[0] = static_cast<uint8_t>(rssi);
        message.data[1] = static_cast<uint8_t>(snr);
        message.len = 2;
        message.to = from;
        message.from = PARMS.parameters.address;
        message.headerID = headerId;
        message.flags = 0; /* Placholder for flags */
        message.transmitTime = randomSignalReportSlot ? getRandomSlot() : getDeterministicSlot();
        addGrid6LocatorIntoMsg(&message);
        MUTEX_LOCK(transmitQueueMutex);
        if (!transmit_queue.isFull()) {
          transmit_queue.enqueue(message, message.transmitTime);
          MUTEX_UNLOCK(transmitQueueMutex);
        } else {
          MUTEX_UNLOCK(transmitQueueMutex);
          MUTEX_LOCK(csvOutputMutex);
          csv_serial.debug("p2p",(char *)"Transmit queue full\n");
          csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
          MUTEX_UNLOCK(csvOutputMutex);
        }
      //} else {
      } else if (( PARMS.parameters.p2pAddressFilterEnabled && (to == PARMS.parameters.address)) ||
                 (!PARMS.parameters.p2pAddressFilterEnabled)) {    //RRP hack to run in promiscuous mode and let p2p filter
        //we have a signal report for us
        int rssi = (int8_t)buf[0];
        int snr = buf[1];
        receivedMsg.rssi = rssi;
        receivedMsg.snr = snr;
        MUTEX_LOCK(receivedQueueMutex);
        if (!receive_queue.isFull()) {
          receive_queue.enqueue(receivedMsg);
          MUTEX_UNLOCK(receivedQueueMutex);
        } else {
          MUTEX_UNLOCK(receivedQueueMutex);
          MUTEX_LOCK(csvOutputMutex);
          csv_serial.debug("p2p",(char *)"receive_queue full\n");
          csv_telnet.debug("p2p",(char *)"receive_queue full\n");
          MUTEX_UNLOCK(csvOutputMutex); 
        }
      }
    } //if manager.recvfrom
    else {
      MUTEX_UNLOCK(radioHeadMutex);
    }
    MUTEX_LOCK(radioHeadMutex);
    isAvailable = manager.available();
    MUTEX_UNLOCK(radioHeadMutex);
    yield();
  } //while message available
  //log_d("bottom of listenFor Message mode: %d",(int)driver.mode());
}
//--------------------------------------------------------------------------------------------------
TaskHandle_t qabTaskHandle;
void queueABroadcastMsgTask(void *pvParameter)
{
  const TickType_t xFrequency = effective_pause / portTICK_PERIOD_MS;
  log_e("Broadcasting period is %ld ticks",xFrequency);
  while (true) {
    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;
    xLastWakeTime = xTaskGetTickCount ();
    queueABroadcastMsg();
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    if (xWasDelayed > 1)
      log_e("Broadcasting a message was delayed by %ld ticks",xWasDelayed);
  }
}
//--------------------------------------------------------------------------------------------------
TaskHandle_t tqmTaskHandle;
void transmitAQueuedMsgTask(void *pvParameter)
{
  const TickType_t xFrequency = 1000;
  log_d("TransmitQueue period is %ld ticks",xFrequency);
  while (true) {
    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;
    xLastWakeTime = xTaskGetTickCount ();
    transmitAQueuedMsg();
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    if (xWasDelayed > 1)
      log_e("Transmitting queued messages was delayed by %ld ticks",xWasDelayed);
  }
}
//--------------------------------------------------------------------------------------------------
TaskHandle_t itrxTaskHandle;
void IdleToRxTask(void *pvParameter)
{
  const TickType_t xFrequency = 10;
  log_d("IdleToRxTask period is %ld ticks",xFrequency);
  while (true) {
    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;
    xLastWakeTime = xTaskGetTickCount ();

    RHGenericDriver::RHMode currentMode = driver.mode();
    if (currentMode == RHGenericDriver::RHModeIdle) {
      //log_d("SX1262 is in idle (standby) switching to ModeRX");
      driver.setModeRx();
    }

    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    if (xWasDelayed > 1)
      log_e("Transmitting queued messages was delayed by %ld ticks",xWasDelayed);
  }
}
//--------------------------------------------------------------------------------------------------
TaskHandle_t listenMsgTaskHandle;
void listenMsgTask(void *pvParameter)
{
  const TickType_t xFrequency = 1;
  log_d("listenMsgTask period is %ld ticks",xFrequency);
  while (true) {
    TickType_t xLastWakeTime;
    BaseType_t xWasDelayed;
    xLastWakeTime = xTaskGetTickCount ();

    listenForMessage();

    RHGenericDriver::RHMode currentMode = driver.mode();
    if (currentMode == RHGenericDriver::RHModeIdle) {
      //log_d("SX1262 is in idle (standby) switching to ModeRX");
      driver.setModeRx();
    }

    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    if (xWasDelayed > 1)
      log_e("listenForMessage() was delayed by %ld ticks",xWasDelayed);
  }
}
//--------------------------------------------------------------------------------------------------
void p2pStop()
{
  vTaskDelete(p2pTaskHandle);
  vTaskDelete(qabTaskHandle);
  vTaskDelete(tqmTaskHandle);
  vTaskDelete(listenMsgTaskHandle);
}
//--------------------------------------------------------------------------------------------------
void p2pSetup(bool broadcastOnlyArg) 
{
  // every PAUSE seconds add a broadcast message to the message queue to be sent
  // if short_aouse is true then the pause interval is cut by half
  // this is to facilitate testing
  if (short_pause) {
    effective_pause = PAUSE * 1000 / 2;
  }
  else {
    effective_pause = PAUSE * 1000;
  }
;
  MUTEX_INIT(csvOutputMutex);
  MUTEX_INIT(receivedQueueMutex);
  MUTEX_INIT(transmitQueueMutex);
  MUTEX_INIT(radioHeadMutex);
  xTaskCreatePinnedToCore(p2pTaskDisplayCSV,"P2PTaskDisplayCSV",10000,NULL,2,&p2pTaskHandle, xPortGetCoreID());
  if (!broadcastOnlyArg)
    xTaskCreatePinnedToCore(queueABroadcastMsgTask,"P2PTaskQABM",10000,NULL,2,&qabTaskHandle, xPortGetCoreID());
  xTaskCreatePinnedToCore(transmitAQueuedMsgTask,"P2PTaskTQM",10000,NULL,2,&tqmTaskHandle, xPortGetCoreID());
  xTaskCreatePinnedToCore(listenMsgTask,"P2PTaskLMT",10000,NULL,1,&listenMsgTaskHandle, xPortGetCoreID());

  //log_d("BroadcastOnly is %s", broadcastOnlyArg ? "On" : "Off");
  //log_e("testStruct_t size is %d",sizeof(testStruct_t));
  //log_e("testPackedStruct_t size is %d",sizeof(testPackedStruct_t));
  //log_e("sizeof(unsigned long): %d sizeof(int) %d sizeof(size_t) %d sizeof(long long) %d",sizeof(unsigned long), sizeof(int),sizeof(size_t),sizeof(long long));
}
//--------------------------------------------------------------------------------------------------
//unsigned long lastLoop = micros();
void p2pLoop(void)
{
  //log_e("loop time %ld in micros",micros() - lastLoop);
  //lastLoop = micros();
  //unsigned long topOfLoop = micros();
  //log_e("Loop time is %ld", micros() - topOfLoop);
  //yield();
  //vTaskDelay(1);
}
//--------------------------------------------------------------------------------------------------
void broadcastOnlyLoop()
{
  vTaskDelay(10000); //Initially delay 10 seconds to get up to speed on GPS and time.
  //MUTEX_LOCK(transmitQueueMutex);
  for (uint8_t i = 1; i < 30; i++) {
    queueABroadcastMsg(i,(i*10)+millis()+10000);
    vTaskDelay(100);
  }
  //MUTEX_UNLOCK(transmitQueueMutex);
  vTaskDelay(1);
}

void debugMessage(char* message)
{
  MUTEX_LOCK(csvOutputMutex);
  char ct[20];
  sprintf(ct," %10ld, -, ", GPS.getTimeStamp());
  csv_serial.debug(ct,message);
  csv_telnet.debug(ct,message);
  MUTEX_UNLOCK(csvOutputMutex);  
}
//--------------------------------------------------------------------------------------------------