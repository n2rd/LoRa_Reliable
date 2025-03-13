#include "main.h"
#include "p2p.h"
#include <pthread.h>
#include "ArduinoQueue.h"
#include "reversePriorityQueue.h"
//
//Peer to Peer Messaging
//
//#define DEBUG  1 //comment this line out for production
//#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       PARMS.parameters.tx_interval // client, time between transmissions
#define TIMEOUT     200  //for sendtoWait

// some state variables
extern bool menu_active;
bool short_pause = false;
unsigned long effective_pause = 0;

// Dont put this on the stack: 
uint8_t buf[DRIVER_MAX_MESSAGE_LEN];

//message management
//uint16_t counter = 0;
uint64_t broadcast_time = 0;

//button presses
uint32_t single_button_time = 0.0;
uint32_t double_button_time = 0.0;

typedef struct {
  uint32_t transmitTime;
  uint8_t to;
  uint8_t from;
  uint8_t len;
  uint8_t headerID;
  uint8_t data[DRIVER_MAX_MESSAGE_LEN];
  char    gridLocator[11];
} transmitMessage_t;


typedef struct {
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
#define MAX_QUEUE 50  //max queue size
ReversePriorityQueue<transmitMessage_t> transmit_queue(MAX_QUEUE);
ArduinoQueue<recvMessage_t> receive_queue(MAX_QUEUE);

uint8_t transmit_headerId = 0;

//keep track of the last time a message was transmitted
uint64_t tx_time = 0;

void extractGrid6LocatorFromData(int startMsgDataIndex, uint8_t* data, int dataLen, char* locator);

//messages are transmitted from the queue
// to transmit a message, add it to the queue
//transmit top item in queue after random delay
#define MAX_DELAY 0x1000 //8192 ms max, must be power of 2
uint16_t random_delay;
//--------------------------------------------------------------------------------------------------
//random delay generator
uint64_t random_delay_generator(uint64_t max) //max must be a power of 2
{
  return esp_random() & max;  //8192 ms max
}
//--------------------------------------------------------------------------------------------------
uint64_t getRandom300msecSlot();

TaskHandle_t p2pTaskHandle;
#define DECLARE_MUTEX(X) pthread_mutex_t X;						   
#define MUTEX_INIT(X) pthread_mutex_init(&X, NULL)
#define MUTEX_LOCK(X) pthread_mutex_lock(&X)
#define MUTEX_UNLOCK(X) pthread_mutex_unlock(&X)

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
  if (count > 2) {
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
uint64_t getRandom300msecSlot()
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
        bool mgrRet = manager.sendto(message.data, message.len, message.to);
        MUTEX_UNLOCK(radioHeadMutex);
        if (!mgrRet)
          log_e("manager.sendto(..) failed");
        MUTEX_LOCK(radioHeadMutex);
        while(driver.mode() == RHGenericDriver::RHModeTx) delayMicroseconds(100);
        RHGenericDriver::RHMode curMode = driver.mode();
        //log_d("Mode after TX is %d", curMode);
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
  bool isAvailable = manager.available();
  MUTEX_UNLOCK(radioHeadMutex);
  while (isAvailable) { //message has come in
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;
    uint8_t id;
    uint8_t flags;
    MUTEX_LOCK(radioHeadMutex);
    while (manager.recvfrom(buf, &len, &from, &to, &id, &flags)) {
      uint8_t headerId = driver.headerId();
      recvMessage_t receivedMsg;
      receivedMsg.timeStamp = GPS.getTimeStamp();
      memcpy(receivedMsg.packet,buf,len);
      receivedMsg.len = len;
      receivedMsg.to = to;
      receivedMsg.from = from;
      receivedMsg.id = id;
      receivedMsg.flags = flags;
      receivedMsg.headerID = headerId;
      if (to == RH_BROADCAST_ADDRESS) {
        //we have a broadcast message, so send reply back to sender
        //and display the broadcast message
        int snr = driver.lastSNR();
        int rssi = driver.lastRssi();
        receivedMsg.snr = snr;
        receivedMsg.rssi = rssi;
        MUTEX_LOCK(receivedQueueMutex);
        if (!transmit_queue.isFull()) {
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
        message.transmitTime = getRandom300msecSlot();
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
      } else {
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
    } //received a message
    MUTEX_UNLOCK(radioHeadMutex);
    MUTEX_LOCK(radioHeadMutex);
    isAvailable = manager.available();
    MUTEX_UNLOCK(radioHeadMutex);
  } //while message available
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
    //vTaskDelay(PAUSE / portTICK_PERIOD_MS);
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
    //vTaskDelay(PAUSE / portTICK_PERIOD_MS);
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
  //random delay for the next transmisson
  random_delay = random_delay_generator(MAX_DELAY);
  MUTEX_INIT(csvOutputMutex);
  MUTEX_INIT(receivedQueueMutex);
  MUTEX_INIT(transmitQueueMutex);
  xTaskCreatePinnedToCore(p2pTaskDisplayCSV,"P2PTaskDisplayCSV",10000,NULL,2,&p2pTaskHandle, xPortGetCoreID());
  if (!broadcastOnlyArg)
    xTaskCreatePinnedToCore(queueABroadcastMsgTask,"P2PTaskQABM",10000,NULL,2,&qabTaskHandle, xPortGetCoreID());
  xTaskCreatePinnedToCore(transmitAQueuedMsgTask,"P2PTaskTQM",10000,NULL,2,&tqmTaskHandle, xPortGetCoreID());

  log_d("BroadcastOnly is %s", broadcastOnlyArg ? "On" : "Off");

  //xTaskCreatePinnedToCore(IdleToRxTask,"P2PTaskITR",5000,NULL,2,&itrxTaskHandle, xPortGetCoreID());
  /*
  ReversePriorityQueue<uint64_t> testQueue(MAX_QUEUE);
  log_d("Starting  enqueue test");
  int count = 30;
  do {
    //uint32_t ran = esp_random();
    uint64_t ran = getRandom300msecSlot();
    log_d("%02d enqueue random value: %llu",count, ran);
    testQueue.enqueue(ran,ran);
  } while (count--);
  log_d("Starting dequeue test");
  count = 1;
  while (!testQueue.isEmpty()) {
    uint64_t ran = testQueue.dequeue();
    log_d("%02d dequeue random value: %llu",count++, ran);
  }
  log_d("Done Queue test");
  */
}
//--------------------------------------------------------------------------------------------------
//unsigned long lastLoop = micros();
void p2pLoop(void)
{
  //log_e("loop time %ld in micros",micros() - lastLoop);
  //lastLoop = micros();
  //unsigned long topOfLoop = micros();
  //listening for others


  listenForMessage();

  //Queue up a Broadcast Message to be sent if it's at it's send interval
  //queueABroadcastMsg();

  //transmit the top message in the queue after random delay
  //transmitAQueuedMsg();
  //log_e("Loop time is %ld", micros() - topOfLoop);
  //yield();
  vTaskDelay(1);
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
  char ct[12];
  sprintf(ct," %ld", GPS.getTimeStamp());
  csv_serial.debug(ct,message);
  csv_telnet.debug(ct,message);
  MUTEX_UNLOCK(csvOutputMutex);  
}
//--------------------------------------------------------------------------------------------------