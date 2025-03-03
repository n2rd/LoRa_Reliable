#include "main.h"
#include "p2p.h"

//
//Peer to Peer Messaging
//
//#define DEBUG  1 //comment this line out for production
//#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
#define TIMEOUT     200  //for sendtoWait

// some state variables
extern bool menu_active;
bool tx_lock = false;
bool short_pause = false;

// Dont put this on the stack: 
uint8_t buf[DRIVER_MAX_MESSAGE_LEN];

//message management
uint16_t counter = 0;
uint64_t broadcast_time = 0;

//button presses
uint32_t single_button_time = 0.0;
uint32_t double_button_time = 0.0;

//queue for transmit
#include "ArduinoQueue.h"
#define MAX_QUEUE 50  //transmit queue size

typedef struct {
  uint8_t to;
  uint8_t len;
  uint8_t headerID;
  uint8_t data[DRIVER_MAX_MESSAGE_LEN];
} message_t;

ArduinoQueue<message_t> transmit_queue(MAX_QUEUE);
uint8_t transmit_headerId = 0;

//keep track of the last time a message was transmitted
uint64_t tx_time = 0;

//messages are transmitted from the queue
// to transmit a message, add it to the queue
//transmit top item in queue after random delay
#define MAX_DELAY 0x1000 //8192 ms max, must be power of 2
uint16_t random_delay;

//random delay generator
uint64_t random_delay_generator(uint64_t max) //max must be a power of 2
{
  return esp_random() & max;  //8192 ms max
}

void p2pSetup(void) 
{
  //random delay for the next transmisson
  random_delay = random_delay_generator(MAX_DELAY);
}

#define GPS_FIX_TIMEOUT 10000

void addGrid6LocatorIntoMsg(message_t* messagePtr, char **gridLocatorPtr = NULL)
{
  const int gridSize = 6;
  double lat = 0, lon = 0;
  bool hasFix;
  if (GPS.onoffState() == GPS.GPS_OFF) {
      log_e("GPS powered off - using fixed location");
      char* fixedMaidenheadGrid;
      fixedMaidenheadGrid = 
        GPS.latLonToMaidenhead(
          PARMS.parameters.lat_value,
          PARMS.parameters.lon_value,
          6
          );
      log_e("Fixed lat %lf, lon %lf, grid %s",PARMS.parameters.lat_value,PARMS.parameters.lon_value,fixedMaidenheadGrid);
      if (gridLocatorPtr != NULL)
        *gridLocatorPtr = fixedMaidenheadGrid;
      encode_grid4_to_buffer(fixedMaidenheadGrid,&messagePtr->data[messagePtr->len]);
      messagePtr->len+=2;
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[4];
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[5];
      return;
  }
  else {
    unsigned long beforeFix = millis();
    do {
      hasFix = !GPS.getLocation(&lat,&lon);
    } while(!hasFix || ((millis()-beforeFix) > GPS_FIX_TIMEOUT));
    if (!hasFix || ((lat == 0.0) && (lon == 0.0))) {
      //Couldn't get a fix
      log_e("GPS powered on but no fix in timeout- using fixed");
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

      log_e("Fixed lat %lf, lon %lf, grid %s",PARMS.parameters.lat_value,PARMS.parameters.lon_value,fixedMaidenheadGrid);
      encode_grid4_to_buffer(fixedMaidenheadGrid,&messagePtr->data[messagePtr->len]);
      messagePtr->len+=2;
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[4];
      messagePtr->data[messagePtr->len++] = (uint8_t)fixedMaidenheadGrid[5];
      return;
    }

    char *curMaidenheadGrid = GPS.latLonToMaidenhead(lat,lon, gridSize);
    if (gridLocatorPtr != NULL)
      *gridLocatorPtr = curMaidenheadGrid;
    encode_grid4_to_buffer(curMaidenheadGrid,&messagePtr->data[messagePtr->len]);
    messagePtr->len+=2;
    messagePtr->data[messagePtr->len++] = (uint8_t)curMaidenheadGrid[4];
    messagePtr->data[messagePtr->len++] = (uint8_t)curMaidenheadGrid[5];
  }
}

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

void p2pLoop(void)
{
  //default mode is listening to others
  if (manager.available())  //message has come in
    {
      uint8_t len = sizeof(buf);
      uint8_t from;
      uint8_t to;
      uint8_t id;
      uint8_t flags;
      if (manager.recvfrom(buf, &len, &from, &to, &id, &flags)) {
        uint8_t headerId = driver.headerId();
        if (to == RH_BROADCAST_ADDRESS) {
          //we have a broadcast message, so send reply back to sender
          //display the broadcast message
          int snr = driver.lastSNR();
          int rssi = driver.lastRssi();
          if (!menu_active) {
            display.printf("B %u-%03u #%i RSSI %i\n", from, headerId, (int)(buf[0]*256 + buf[1]), rssi); //Do it all in one
          }
          //add the signal report to the message queue
          message_t message;
          message.data[0] = static_cast<uint8_t>(rssi);
          message.data[1] = static_cast<uint8_t>(snr);
          message.len = 2;
          message.to = from;
          message.headerID = headerId;

          addGrid6LocatorIntoMsg(&message);

          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
          } else {
            csv_serial.debug("p2p",(char *)"Transmit queue full\n");
            csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
          }

          char gridLocator[11];
          extractGrid6LocatorFromData(2, buf, len, gridLocator);
          csv_serial.data(GPS.getTimeStamp(), 'B', from, to, headerId, rssi, snr, gridLocator);
          csv_telnet.data(GPS.getTimeStamp(), 'B', from, to, headerId, rssi, snr, gridLocator);
        } else {
          //we have a signal report for us
          int rssi = (int8_t)buf[0];
          int snr = buf[1];
          if (!menu_active) {
            display.printf("R %u-%03u RSSI %i\n", from, headerId, rssi);
          }
          char gridLocator[11];
          extractGrid6LocatorFromData(2, buf, len, gridLocator);
          csv_serial.data(GPS.getTimeStamp(), 'S', from, to, headerId, rssi, snr, gridLocator);
          csv_telnet.data(GPS.getTimeStamp(), 'S', from, to, headerId, rssi, snr, gridLocator);
        }
      } //received a message
    } //message waiting

  // every PAUSE seconds add a broadcast message to the message queue to be sent
  // if short_aouse is true then the pause interval is cut by half
  // this is to facilitate testing
  int effective_pause = PAUSE;
  if (short_pause) {
    effective_pause = PAUSE / 2;
  }
  if ((!tx_lock) && (((millis() - broadcast_time) > (effective_pause * 1000)))) {
     broadcast_time = millis();
     //add the broadcast message to the message queue
          message_t message;
          message.data[0] = static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
          message.data[1] = static_cast<uint8_t>(counter & 0xFF); //low byte
          message.len = 2;
          message.headerID = ++transmit_headerId;
          message.to = RH_BROADCAST_ADDRESS;
          //Add maidenheadGrid6
          char* gridLocator;
          addGrid6LocatorIntoMsg(&message, &gridLocator);

          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
            uint8_t from = manager.thisAddress();
            csv_serial.broadcast(GPS.getTimeStamp(), from, transmit_headerId, gridLocator);
            csv_telnet.broadcast(GPS.getTimeStamp(), from, transmit_headerId, gridLocator);
          } else {
            csv_serial.debug("p2p",(char *)"Transmit queue full\n");
            csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
          }
     counter++;
   } //broadcast message

  //transmit the top message in the queue after random delay
  if ((!tx_lock) && ((millis() - tx_time) > random_delay)) {
    if (!transmit_queue.isEmpty()) {
      message_t message = transmit_queue.dequeue();
      unsigned long curMicros = micros();
      manager.setHeaderId(message.headerID);
      manager.sendto(message.data, message.len, message.to);
      unsigned long transmitMicros = micros() - curMicros;
      //log_e("Transmit time: %ld", transmitMicros);
      tx_time = millis();
      int max_delay;
      if (!short_pause){
        max_delay = random_delay_generator(MAX_DELAY);
      } else {
        max_delay = random_delay_generator(MAX_DELAY/2);  //short pause
      }
    } //if it is time to transmit a message
  }
} //loop
