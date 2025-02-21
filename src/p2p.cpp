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
  uint8_t data[DRIVER_MAX_MESSAGE_LEN];
} message_t;

ArduinoQueue<message_t> transmit_queue(MAX_QUEUE);

//keep track of the last time a message was transmitted
uint64_t tx_time = 0;

//messages are transmitted from the queue
// to transmit a message, add it to the queue
//transmit top item in queue after random delay
#define MAX_DELAY 0x2000 //8192 ms max, must be power of 2
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
        if (to == RH_BROADCAST_ADDRESS) {
          //we have a broadcast message
          //display the message
          int snr = driver.lastSNR();
          int rssi = driver.lastRssi();
          ps_all.printf("Broadcast from %i #%i\n", from, (int)(buf[1]*256 + buf[0]));
          ps_all.printf("%iB #%i ", from, (int)(buf[1]*256 + buf[0]));
          ps_all.printf("RSSI %i  SNR %i\n", rssi, snr);
          csv_serial.data(millis(),from,rssi,snr);
          csv_telnet.data(millis(),from,rssi,snr);
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
          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
          } else {
            csv_serial.debug("p2p",(char *)"Transmit queue full\n");
            csv_telnet.debug("p2p",(char *)"Transmit queue full\n");
          }
          //manager.sendto(data, 2, from);
        } else {
          //we have a signal report for us
          ps_all.printf("Signal report from %i\n", from);
          ps_all.printf("RSSI -%i SNR %i\n", (int)buf[0], (int)buf[1]);
          csv_serial.data(millis(),from,buf[0],buf[1]);
          csv_telnet.data(millis(),from,buf[0],buf[1]);
        }
      } //received a message
    } //message waiting

  // every PAUSE seconds add a broadcast message to the message queue to be sent
  if (millis() - broadcast_time > PAUSE * 1000) {
     broadcast_time = millis();
     data[0] = static_cast<uint8_t>(counter & 0xFF); //low byte
     data[1] = static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
     //add the broadcast message to the message queue
          message_t message;
          message.data[0] = data[0];
          message.data[1] = data[1];
          message.len = 2;
          message.to = RH_BROADCAST_ADDRESS;
          if (!transmit_queue.isFull()) {
            transmit_queue.enqueue(message);
          } else {
            csv_serial.debug("p2p",(char *)"Transmit queue full");
            csv_telnet.debug("p2p",(char *)"Transmit queue full");
          }
     counter++;
   } //broadcast message

  //transmit the top message in the queue after random delay
  if (millis() - tx_time > random_delay) {
    if (!transmit_queue.isEmpty()) {
      message_t message;
      message = transmit_queue.dequeue();
      manager.sendto(message.data, message.len, message.to);
      tx_time = millis();
      random_delay = random_delay_generator(MAX_DELAY);
    }
  } //if it is time to transmit a message
} //loop
