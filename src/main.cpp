#include <Arduino.h>
// Example sketch showing how to create a simple peer to peer messaging network, single hop
// with acknowledgements to RF95_reliable_datagram_client and server
// Contributed by Charles-Henri Hallard based on sample RF95_reliable_datagram_client and server
// and RF95_reliable_datagram_client and server by Mike Poublon
// and ropg for the unofficial Heltec library

// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_DEFAULT_POWER_BUTTON   // must be before "#include <myHeltec.h>"

//version
#define VERSION __DATE__  " "  __TIME__


// SETUP Parameters
//
#define MY_ADDRESS 11   //open

//
//Peer to Peer Messaging
//
//#define DEBUG  1 //comment this line out for production
#define DEFAULT_FREQUENCY 915.0
#define DEFAULT_POWER_INDEX 0     //see table below, index 0 is -9dBm, index 6 is +22dBm max 
#define DEFAULT_MODULATION_INDEX 5      //see LoRa settings table below

//#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
#define TIMEOUT     200  //for sendtoWait
#define RETRIES     3     //for sendtoWait

#include <SPI.h>
#include <RH_SX126x.h>

RH_SX126x driver(8, 14, 13, 12); // NSS, DIO1, BUSY, NRESET

#include "myHeltec.h" //must come after driver declaration

//
// LoRa settings that are used for Meshtastic
//
// No Channel Name	 / Data-Rate	/ SF/ Symb	/ Coding Rate	/ Bandwidth	/ Link Budget
// 0  Short Turbo	   / 21.88 kbps /	7 / 128	  / 4/5	        / 500 kHz	  / 140dB
// 1  Short Fast	   / 10.94 kbps / 7 / 128	  / 4/5	        / 250 kHz	  / 143dB
// 2  Short Slow	   / 6.25 kbps	/ 8 / 256	  / 4/5	        / 250 kHz	  / 145.5dB
// 3  Medium Fast	   / 3.52 kbps	/ 9 / 512	  / 4/5	        / 250 kHz	  / 148dB
// 4  Medium Slow	   / 1.95 kbps	/ 10 / 1024	/ 4/5	        / 250 kHz	  / 150.5dB
// 5  Long Fast	     / 1.07 kbps	/ 11 / 2048	/ 4/5	        / 250 kHz	  / 153dB
// 6  Long Moderate  / 0.34 kbps	/ 11 / 2048	/ 4/8	        / 125 kHz	  / 156dB
// 7  Long Slow	     / 0.18 kbps	/ 12 / 4096	/ 4/8	        / 125 kHz	  / 158.5dB
// 8  Very Long Slow / 0.09 kbps	/ 12 / 4096 /	4/8	        / 62.5 kHz	/ 161.5dB

// These are indexed by the values of ModemConfigChoice
// Stored in flash (program) memory to save SRAM
PROGMEM static const RH_SX126x::ModemConfig MY_MODEM_CONFIG_TABLE[] =
{
    //  packetType, p1, p2, p3, p4, p5, p6, p7, p8
     // 0 Short Turbo
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_128, RH_SX126x_LORA_BW_500_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 1 Short Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_128, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 2 Short Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_256, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 3 Medium Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_512, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 4 Medium Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_1024, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 5 Long Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 6 Long Moderate
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 7 Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 8 Very Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_62_5, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
};

#define MODULATION_INDEX_MAX 9
static const String MY_CONFIG_NAME[MODULATION_INDEX_MAX] =
{
"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"
};
int modulation_index = DEFAULT_MODULATION_INDEX;

#define POWER_INDEX_MAX 7
float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};
int power_index = DEFAULT_POWER_INDEX;

#include "RHDatagram.h"

RHDatagram manager(driver, MY_ADDRESS);

//send and receive data
//uint8_t data[] = "And hello back to you";
// transmit buffer
uint8_t data[20];
// Dont put this on the stack: 
// it is fragile, you will break it if you touch it
// do not rename, etc.,  if you mess with it, you won't get anything that is received
uint8_t buf[RH_SX126x_MAX_MESSAGE_LEN];

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
  uint8_t data[RH_SX126x_MAX_MESSAGE_LEN];
} message_t;

ArduinoQueue<message_t> transmit_queue(MAX_QUEUE);

//keep track of the last time a message was transmitted
uint64_t tx_time = 0;

//messages are transmitted from the queue
// to transmit a message, add it to the queue
//transmit top item in queue after random delay
#define MAX_DELAY 0x2000 //8192 ms max, must be power of 2
uint16_t random_delay;

//
// funtion declarations
//
void check_button();
uint64_t random_delay_generator(uint64_t max);

void setup() 
{
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available

  delay(5000);

  //display init
  heltec_display_power(true);
  heltec_ve(true); //added to turn on display
  display.init();
  display.setContrast(255);
  display.flipScreenVertically();


  //start the radio
  if (manager.init()) 
  {
    display.printf("V %s\n", VERSION); 
  } else {
    display.println("Radio failed to initialize");
  }

  display.printf("Unit #%i at %.1f MHz\n", MY_ADDRESS, DEFAULT_FREQUENCY);
  display.printf("%s %.1f dBm\n", MY_CONFIG_NAME[modulation_index], power[power_index]);

  driver.setFrequency(DEFAULT_FREQUENCY);
  
  RH_SX126x::ModemConfig cfg;
  memcpy_P(&cfg, &MY_MODEM_CONFIG_TABLE[modulation_index], sizeof(RH_SX126x::ModemConfig));
  driver.setModemRegisters(&cfg);
  driver.setTxPower(power[power_index]);
  
  //You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
//  driver.setCADTimeout(DEFAULT_CAD_TIMEOUT);  //Carrier Activity Detect Timeout 

  // Battery
  float vbat = heltec_vbat();
  display.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));

  //random delay for the next transmisson
  random_delay = random_delay_generator(MAX_DELAY);
}

void loop()
{
  //first check the buttons
  check_button();

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
          both.printf("Broadcast from %i #%i\n", from, (int)(buf[1]*256 + buf[0]));
          //both.printf("%iB #%i ", from, (int)(buf[1]*256 + buf[0]));
          both.printf("RSSI %i  SNR %i\n", rssi, snr);
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
            both.println("Transmit queue full");
          }
          //manager.sendto(data, 2, from);
        } else {
          //we have a signal report for us
          both.printf("Signal report from %i\n", from);
          both.printf("RSSI -%i SNR %i\n", (int)buf[0], (int)buf[1]);
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
            both.println("Transmit queue full");
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

//random delay generator
uint64_t random_delay_generator(uint64_t max) //max must be a power of 2
{
  return esp_random() & max;  //delay in ms; max must be a power of 2
}

void check_button() 
{
  button.update();

  //single click to change power
  if (button.isSingleClick()) 
  {
    uint32_t button_time = millis() - single_button_time;
    single_button_time = millis();
    //if button time is < 1000 then pick next value
    //else show current value
    if (button_time > 2000 ) 
    {
      both.printf("Current Power %.1fdBm\n", power[power_index]);
      both.println("Single press button to change\n");
    } else {
      power_index = (power_index + 1) % POWER_INDEX_MAX;
      driver.setTxPower(power[power_index]);
      both.printf("New Power %.1fdBm\n", power[power_index]);
    }
  }
 
  //double click inits the radio
  if (button.isDoubleClick()) // start the radio 
  {
    uint32_t button_time = millis() - double_button_time;
    double_button_time = millis();
    //if button time is < 1000 then pick next value
    //else show current value
    if (button_time > 2000 ) 
    {
      both.printf("Current Modulation %i %s\n", MY_CONFIG_NAME[modulation_index]);
      both.println("Double press button to change\n");
    } else {
      modulation_index = (modulation_index + 1) % MODULATION_INDEX_MAX;
      RH_SX126x::ModemConfig cfg;
      memcpy_P(&cfg, &MY_MODEM_CONFIG_TABLE[modulation_index], sizeof(RH_SX126x::ModemConfig));
      driver.setModemRegisters(&cfg);
      both.printf("Current Modulation %i %s\n", MY_CONFIG_NAME[modulation_index]);
    }
  }

  //long press puts to sleep
  if (button.pressedFor(1000)) 
  { //go to sleep
    // Visually confirm it's off so user releases button
    display.displayOff();
    // Deep sleep (has wait for release so we don't wake up immediately)
    heltec_deep_sleep();
  }
}