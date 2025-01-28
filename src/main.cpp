#include <Arduino.h>
// sx1262_server.ino
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_SX126x class and a basic SX1262 module connected to an Arduino compatible processor
// It is designed to work with the examples stm32wlx_client and sx1262_client.
// Tested with G-Nice LoRa1262-915 and Teensy 3.1

// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_DEFAULT_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"

//
// SETUP Parameters
//
#define ADDRESS_MAX 4 //use 1 for server, others are all clients
#define SERVER_ADDRESS 1  
#define MY_ADDRESS 3
#define DEFAULT_FREQUENCY 905.2
#define DEFAULT_POWER_INDEX 6     //22 dBm see table below
#define DEFAULT_MODULATION_INDEX 7      //see LoRa settings table below
#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
#define TIMEOUT   200  //for sendtoWait
#define RETRIES   3     //for sendtoWait

#include <SPI.h>
#include <RH_SX126x.h>

RH_SX126x driver(8, 14, 13, 12); // NSS, DIO1, BUSY, NRESET

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

#include "myHeltec.h"

#include "RHReliableDatagram.h"

RHReliableDatagram manager(driver, MY_ADDRESS);

//send and receive data
//uint8_t data[] = "And hello back to you";
// transmit buffer
uint8_t data[20];
// Dont put this on the stack: 
// it is fragile, you will break it if you touch it
// do not rename, etc.,  if you mess with it, you won't get anything that is received
uint8_t buf[RH_SX126x_MAX_MESSAGE_LEN];

//message management
long counter = 0;
uint64_t tx_time = 0;

//button presses
uint32_t single_button_time = 0.0;
uint32_t double_button_time = 0.0;

//
// funtion declarations
//
void check_button();

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


  //set up the radio with default values
  power_index = DEFAULT_POWER_INDEX;
  modulation_index = DEFAULT_MODULATION_INDEX;
  both.printf("Starting Radio at %.1f MHz\n", DEFAULT_FREQUENCY);
  both.printf("%s %.1f dBm\n", MY_CONFIG_NAME[modulation_index], power[power_index]);
  driver.setFrequency(DEFAULT_FREQUENCY);
  //default modulation, get details from PROGMEM
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
  both.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));

  //start the radio
  if (manager.init()) 
  {
    both.printf("Radio Started as #%i\n", MY_ADDRESS); 
  } else {
    both.println("Radio failed to initialize");
  }

//messages
//   both.println("Single click to change power");
//   both.println("Double click to change modulation");
}

void loop()
{
  //first check the buttons
  check_button();

  //now operate in different roles
  if (MY_ADDRESS == 1)  //serving as a server
  {
    if (manager.available())  //true if data is available
    {
      // Wait for a message addressed to us from the client
      uint8_t len = sizeof(buf);
      uint8_t from;
      manager.recvfromAck(buf, &len, &from);
      int snr = driver.lastSNR();
      int rssi = driver.lastRssi();
      both.printf("From %i: %s\n",from, (char*)buf);
      both.printf("RSSI: %i   SNR: %i\n", rssi, snr);
      //now send a report back to the client
      sprintf((char *)data, "RSSI %i SNR %i\n", rssi, snr);
      manager.sendto(data, sizeof(data), from);
    }
  }  
  
  if (MY_ADDRESS > 1)  //serving as a client
  {  
    // Send a message to manager_server
    if (millis() - tx_time > PAUSE * 1000) 
    {
      tx_time = millis();
      sprintf((char *)data, "%i", counter);
      if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
      {
        both.printf("Sent %s\n", data);
      } else {
        both.println("sendtoWait failed");
      }
      counter++;
      uint8_t len = sizeof(buf);
      uint8_t from = 0;
      manager.recvfromAckTimeout(buf, &len, 2000, &from);
      if (from != 0)
        both.printf("From %i: %s\n",from, (char*)buf);
    } //legal to transmit
  } // as a client
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
