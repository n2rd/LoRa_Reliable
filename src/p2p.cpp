#include "main.h"

//
//Peer to Peer Messaging
//
//#define DEBUG  1 //comment this line out for production
//#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
#define TIMEOUT     200  //for sendtoWait


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
          display.printf("Broadcast from %i #%i\n", from, (int)(buf[1]*256 + buf[0]));
          display.printf("%iB #%i ", from, (int)(buf[1]*256 + buf[0]));
          display.printf("RSSI %i  SNR %i\n", rssi, snr);
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
          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
          } else {
            csv_serial.debug("p2p",(char *)"Transmit queue full\n");
            csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
          }
          csv_serial.data(millis(), 'B', from, to, headerId, rssi, snr);
          csv_telnet.data(millis(), 'B', from, to, headerId, rssi, snr);
        } else {
          //we have a signal report for us
          int rssi = 0 - buf[0];
          int snr = buf[1];
          display.printf("SigRep %u ", from);
          display.printf("RSSI %i SNR %i\n", rssi, snr);
          csv_serial.data(millis(), 'S', from, to, headerId, rssi, snr);
          csv_telnet.data(millis(), 'S', from, to, headerId, rssi, snr);
        }
      } //received a message
    } //message waiting

  // every PAUSE seconds add a broadcast message to the message queue to be sent
  if ((!tx_lock) && (((millis() - broadcast_time) > (PAUSE * 1000)))) {
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
          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
            uint8_t from = manager.thisAddress();
            csv_serial.broadcast(millis(),from,transmit_headerId);
            csv_telnet.broadcast(millis(),from,transmit_headerId);
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
