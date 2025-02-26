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


//send and receive data
//uint8_t data[] = "And hello back to you";
// transmit buffer
uint8_t data[20];
// Dont put this on the stack: 
// it is fragile, you will break it if you touch it
// do not rename, etc.,  if you mess with it, you won't get anything that is received
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

#define GPS_FIX_TIMEOUT 1000

void addGrid6LocatorIntoMsg(message_t* messagePtr, char **gridLocatorPtr = NULL)
{
  const int gridSize = 8;
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
      messagePtr->data[messagePtr->len] = (uint8_t)fixedMaidenheadGrid[5];
      return;
  }
  else {
    log_e("going into GPS powered on");
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
      messagePtr->data[messagePtr->len] = (uint8_t)fixedMaidenheadGrid[5];
      return;
    }
    log_e("Got a fix TTF %lu lat %lf lon %lf",millis()-beforeFix, lat,lon);
    ///////////// TESTING ///////////////
    //lat = 43.147195; lon = -76.171003; //Ron's house FN13VD95LH Dif -0.334576 -0.669267
    //lat = 0.000; lon = 180.000; 
    char *curMaidenheadGrid = GPS.latLonToMaidenhead(lat,lon, gridSize);
    if (gridLocatorPtr != NULL)
      *gridLocatorPtr = curMaidenheadGrid;
    encode_grid4_to_buffer(curMaidenheadGrid,&messagePtr->data[messagePtr->len]);
    messagePtr->len+=2;
    messagePtr->data[messagePtr->len++] = (uint8_t)curMaidenheadGrid[4];
    messagePtr->data[messagePtr->len] = (uint8_t)curMaidenheadGrid[5];
    ///////////// TESTING ///////////////
    //log_e("Grid: %s",curMaidenheadGrid);
    //double newLat = 0, newLon = 0;
    //GPS.maidenheadGridToLatLon(curMaidenheadGrid,&newLat, &newLon);
    //log_e("Original: %lf %lf Converted: %lf %lf Dif: %lf %lf",lat, lon, newLat, newLon,lat-newLat,lon-newLon);
    //Difference for 43.024377 -76.202852 converted to FN13va55pu is -1.338817 -2.677505
    //free(curMaidenheadGrid);
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
          //we have a broadcast message
          //display the message
          int snr = driver.lastSNR();
          int rssi = driver.lastRssi();
          if (!menu_active) {
            //display.printf("Broadcast from %i #%i\n", from, (int)(buf[1]*256 + buf[0])); //Duplicative commented out
            //display.printf("RSSI %i  SNR %i\n", rssi, snr); //SNR goes off screen so don't display it.
            display.printf("B %u-%03u #%i RSSI %i\n", from, headerId, (int)(buf[1]*256 + buf[0]), rssi); //Do it all in one
          }
          //send reply back to sender
          rssi = abs(rssi);
          data[0] = static_cast<uint8_t>(rssi);
          data[1] = static_cast<uint8_t>(snr);
          //add the signal report to the message queue
          message_t message;
          message.data[0] = data[0];
          message.data[1] = data[1];
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
          //if (gridLocator != NULL)
          //  log_e("Grid Locator: %s", gridLocator);
          char gridLocator[11];
          extractGrid6LocatorFromData(2, buf, len, gridLocator);
          csv_serial.data(GPS.getTimeStamp(), 'B', from, to, headerId, rssi, snr, gridLocator);
          csv_telnet.data(GPS.getTimeStamp(), 'B', from, to, headerId, rssi, snr, gridLocator);
        } else {
          //we have a signal report for us
          int rssi = 0 - buf[0];
          int snr = buf[1];
          if (!menu_active) {
            display.printf("SR %u-%03u RSSI %i\n", from, headerId, rssi);
            //display.printf("Signal Report from %i\n", from);
            //display.printf("SigRep %u ", from);
            //display.printf("RSSI %i SNR %i\n", rssi, snr);
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
     data[0] = static_cast<uint8_t>(counter & 0xFF); //low byte
     data[1] = static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
     //add the broadcast message to the message queue
          message_t message;
          message.data[0] = data[0];
          message.data[1] = data[1];
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
    //log_e("ATransmit Queue Depth: %d", transmit_queue.itemCount());
    if (!transmit_queue.isEmpty()) {
      message_t message;
      message = transmit_queue.dequeue();
      //log_e("Queue Depth: %u",transmit_queue.itemCount());
      unsigned long curMicros = micros();
      manager.setHeaderId(message.headerID);
      manager.sendto(message.data, message.len, message.to);
      unsigned long transmitMicros = micros() - curMicros;
      log_e("Transmit time: %ld", transmitMicros);
      tx_time = millis();
      int max_delay;
      if (!short_pause){
        max_delay = random_delay_generator(MAX_DELAY);
      } else {
        max_delay = random_delay_generator(MAX_DELAY/2);  //short pause
      }
    } //if it is time to transmit a message
  }
  else {
    //log_e("BTransmit Queue Depth: %d", transmit_queue.itemCount());
  }
} //loop
