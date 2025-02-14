#include "main.h"

#ifdef ARDUINO_LILYGO_T3_V1_6_1
  #include "myLilyGoT3.h"
#else
  // Turns the 'PRG' button into the power button, long press is off 
  #define HELTEC_DEFAULT_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
  #include "myHeltec.h"
#endif

#if defined(USE_WIFI) && (USE_WIFI >0)
  #if defined(ESP32)
    #if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
      AsyncWebServer server(80);
    #else
      WebServer server(80);
    #endif
  #endif //defined(ESP32)
#endif //deefined(USE_WIFI) && (USE_WIFI > 0)



// 2 bytes packet payload, logging data as csv on serial port
/***  logging format ***
*  server
*     millis, from, counter, rssi, snr, send_report_back_status
* 
*  client
*   successful
*     millis, counter, rssi, snr, rssi_reported_by_server, snr_report 
*   failed
*     millis, counter, "failed"
*/

//
//EXPERIMENTATION
//
//#define RAJ_DEBUG  1 //comment this line out for production
#ifdef RAJ_DEBUG
  #define DEFAULT_FREQUENCY 915.0
  #define DEFAULT_POWER_INDEX 0     //see table below, index 0 is -9dBm, index 6 is +22dBm max 
  #define DEFAULT_MODULATION_INDEX 5      //see LoRa settings table below
#endif

#ifndef DEFAULT_FREQUENCY
  #define DEFAULT_FREQUENCY 905.2
#endif
#ifndef DEFAULT_POWER_INDEX
  #define DEFAULT_POWER_INDEX 4     //see table below, index 0 is -9dBm, index 6 is +22dBm max 
#endif
#ifndef DEFAULT_MODULATION_INDEX
  #define DEFAULT_MODULATION_INDEX 5      //see LoRa settings table below
#endif

#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions
#define TIMEOUT     200  //for sendtoWait
#define RETRIES     3     //for sendtoWait



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

#define MODULATION_INDEX_MAX 9
// These are indexed by the values of ModemConfigChoice
// Stored in flash (program) memory to save SRAM
#ifndef ARDUINO_LILYGO_T3_V1_6_1
PROGMEM static const RH_SX126x::ModemConfig MY_MODEM_CONFIG_TABLE[MODULATION_INDEX_MAX] =
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
#define MODEMCONFIGSZ sizeof(RH_SX126x::ModemConfig)

void setModemConfig(uint8_t index) {
  if (index > MODULATION_INDEX_MAX)
    index = MODULATION_INDEX_MAX - 1;
    DRIVER_TYPE::ModemConfig cfg;
    #warning "Remove commented lines later"
    //memcpy_P(&cfg, &MY_MODEM_CONFIG_TABLE[modulation_index], MODEMCONFIGSZ);
    //driver.setModemRegisters(&cfg);
    driver.setModemRegisters(&MY_MODEM_CONFIG_TABLE[index]);
}


#define DRIVER_MAX_MESSAGE_LEN RH_SX126x_MAX_MESSAGE_LEN

#else //LILLYGO_T3 RF95 aka SX127x chips
typedef struct modem_config_struct {
  long bandwidth;
  uint8_t spreadingFactor;
  uint8_t codingRate4Denominator;
} MYMODEM_CONFIG_TABLE;

enum SF {
  SF_64 = 6,
  SF_128 = 7,
  SF_256 = 8,
  SF_512 = 9,
  SF_1024 = 10,
  SF_2048 = 11,
  SF_4096 = 12
};

PROGMEM static const MYMODEM_CONFIG_TABLE TTGO_MODEM_CONFIG_TABLE[MODULATION_INDEX_MAX] = 
{
  { 500000, SF_128, 5}, //0 Short Turbo
  { 250000, SF_128, 5}, //1 Short Fast
  { 250000, SF_256, 5}, //2 Short Slow
  { 250000, SF_512, 5}, //3 Medium Fast
  { 250000, SF_1024, 5}, //4 Medium Slow
  { 250000, 11, 5}, // 5 LongFast
  { 125000, 11, 8}, //6 Long Moderate
  { 125000, SF_4096, 8}, //7 Long Slow
  { 062500, SF_4096,8}  //8 Very Long Slow
};

void setModemConfig(uint8_t index) {
  if (index > MODULATION_INDEX_MAX)
    index = MODULATION_INDEX_MAX - 1;
  driver.setSignalBandwidth(TTGO_MODEM_CONFIG_TABLE[index].bandwidth);
  driver.setSpreadingFactor(TTGO_MODEM_CONFIG_TABLE[index].spreadingFactor);
  driver.setCodingRate4(TTGO_MODEM_CONFIG_TABLE[index].codingRate4Denominator);
}

#define DRIVER_MAX_MESSAGE_LEN RH_RF95_MAX_MESSAGE_LEN
#endif

static const String MY_CONFIG_NAME[MODULATION_INDEX_MAX] =
{
"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"
};
int modulation_index = DEFAULT_MODULATION_INDEX;

#define POWER_INDEX_MAX 7
float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};
int power_index = DEFAULT_POWER_INDEX;

PrintSplitter both(Serial, display);

RHReliableDatagram manager(driver, MY_ADDRESS);

// temporary transmit buffer
uint8_t data[20];

// Dont put this on the stack: 
// it is fragile, you will break it if you touch it
// do not rename, etc.,  if you mess with it, you won't get anything that is received
uint8_t buf[DRIVER_MAX_MESSAGE_LEN];

//message management
uint16_t counter = 0;
uint64_t tx_time = 0;

//button presses
uint32_t single_button_time = 0.0;
uint32_t double_button_time = 0.0;

//
// funtion declarations
//
void check_button();



void DisplayUpperRight(int count) {
  char buf[10];
  memset(buf,32,sizeof(buf));
  sprintf(buf+2,"%5d", count);
  display.setColor(BLACK);
  display.fillRect((display.getWidth()) - 32, 0, 32, 14);
  display.setColor(WHITE);
  display.setColor(INVERSE);
  display.drawString((display.getWidth()) - 30, 0, buf);
  display.display();
  display.setColor(INVERSE);
  display.display();
}

void toggleLED()
{
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
}

void setup() 
{
  #ifdef ARDUINO_LILYGO_T3_V1_6_1
  pinMode(LED_BUILTIN, OUTPUT);
  SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_CS);
  #endif
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  delay(5000);
  ota_setup();
  telnet_setup();
  //display init
  #ifndef ARDUINO_LILYGO_T3_V1_6_1
  heltec_display_power(true);
  heltec_ve(true); //added to turn on display
  #endif //!deefined(ARDUINO_LILYGO_T3_V1_6_1)
  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.displayOn();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, (display.getHeight() / 3)-16 /* font height */, "Lora_Reliable");
  display.drawString(display.getWidth() / 2, (display.getHeight() / 3)*2-16 /* font height */, VERSION);
  display.display();
  delay(3000);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.cls();

  //start the radio
  if (!manager.init()) 
  {
    display.println("Radio failed to initialize");
    both.println("HALTING");
    while (1);
  }

  power_index = DEFAULT_POWER_INDEX;
  modulation_index = DEFAULT_MODULATION_INDEX;

  if (MY_ADDRESS == 1) {
    display.printf("Server %.1f MHz\n", DEFAULT_FREQUENCY);
  } else {
    display.printf("Client #%i at %.1f MHz\n", MY_ADDRESS, DEFAULT_FREQUENCY);
  }
  display.printf("%s %.1f dBm\n", MY_CONFIG_NAME[modulation_index], power[power_index]);
  driver.setFrequency(DEFAULT_FREQUENCY);
  //default modulation, get details from PROGMEM
 #ifndef ARDUINO_LILYGO_T3_V1_6_1
  {
    DRIVER_TYPE::ModemConfig cfg;
    memcpy_P(&cfg, &MY_MODEM_CONFIG_TABLE[modulation_index], MODEMCONFIGSZ);
    driver.setModemRegisters(&cfg);
  }
#else
//TBD: Pull this from a table that matches the heltec boards Spreading factor, Bandwidth and Symbol Rate
//This is LONG_FAST
  driver.setSignalBandwidth(250000);
  driver.setSpreadingFactor(11);
  driver.setCodingRate4(5);
#endif
  driver.setTxPower(power[power_index]);
  #define DEBUG_INCOMING_PACKETS
  #if defined(DEBUG_INCOMING_PACKETS) && defined(ARDUINO_LILYGO_T3_V1_6_1)
  //driver.setPromiscuous(true);
  #endif
  //You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
#ifdef ARDUINO_LILYGO_T3_V1_6_1
  driver.setPayloadCRC(true);
  driver.setCADTimeout(DEFAULT_CAD_TIMEOUT);  //Carrier Activity Detect Timeout 
#else
// This doesn't work on the heltec SX1262 driver yet
#endif

  // Battery
  float vbat = heltec_vbat();
  display.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));
//messages
//   both.println("Single click to change power");
//   both.println("Double click to change modulation");
//lets query it all back from the receiver
}

int Failure_Counter = 0;
int debugLoopCount = 0;
RHGenericDriver::RHMode lastMode = (RHGenericDriver::RHMode)-1;

void loop()
{
  //first check the buttons
  check_button();
  ota_loop();
  telnet_loop();
  //now operate in different roles
  if (MY_ADDRESS == 1)  //serving as a server
  {
    driver.setModeRx();
    if (driver.mode() != lastMode) {
      Serial.printf("driver.mode changed to %d\n",driver.mode());
      lastMode = driver.mode();
      DisplayUpperRight(lastMode);
    }
    else {
      //Serial.println("Driver mode not changed");
    }

    if (manager.available())  //message has come in
    {
      Serial.printf("Got a Message. Server Count= %d\n",debugLoopCount++);
      // Wait for a message addressed to us from the client
      uint8_t len = sizeof(buf);
      uint8_t from = 0;
      uint8_t to = 0;
      uint8_t id = 0;
      uint8_t flags = 0xFF;
      if (manager.recvfromAck(buf, &len, &from, &to, &id, &flags))
      {
        int snr = driver.lastSNR();
        int rssi = driver.lastRssi();
        display.printf("%i -> %i\n", from, (int)(buf[1]*256 + buf[0]));
        display.printf("RSSI %i   SNR %i flags: %i\n", rssi, snr, flags);
        rssi = abs(rssi);
        data[0] = static_cast<uint8_t>(rssi);
        data[1] = static_cast<uint8_t>(snr);
        Serial.printf("%i, %i, %i, -%i, %i, %d\n", millis(), from, (int)(buf[1]*256 + buf[0]), rssi, snr, manager.retransmissions());
        manager.resetRetransmissions();
        if (!manager.sendtoWait(data, 2, from)) {
          //display.println("sendtoWait failed");
          Serial.printf("sendtoWait to %d failed in %s %d\n", from,__FILE__,__LINE__);
          DisplayUpperRight(++Failure_Counter);
        }
        else
          DisplayUpperRight(Failure_Counter);
      }
      else {
        //We can get here because there was no packet received, it wasn't for us or was an ACK
        Serial.printf("manager.recvfromAck FAILED flags= %X at Line %d in %s\n", flags,__LINE__,__FILE__);
      }
    }
  }  //address 1 SERVER

  if (MY_ADDRESS > 1)  //serving as a client
  {  
    // Send a message to manager_server
    if ((millis() - tx_time) > (PAUSE * 1000)) 
    {
      tx_time = millis();
      data[0] = static_cast<uint8_t>(counter & 0xFF); //low byte
      data[1] = static_cast<uint8_t>((counter >> 8) & 0xFF); //highbyte
      manager.resetRetransmissions();
      Serial.printf("before manager.sendtowait line %d in %s\n",__LINE__,__FILE__);
      if (manager.sendtoWait((uint8_t *)data, 2, SERVER_ADDRESS))
      {
        int retransmisison_count = manager.retransmissions();
        display.print("Sent ");
        display.print((int)(data[1]*256 + data[0]));
        display.printf(" retrans = %i\n", retransmisison_count);

        // Now wait for a reply from the server
        uint8_t len = sizeof(buf);
        uint8_t from;   
        if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
        {
          display.printf("1 -> RSSI -%i SNR %i\n", (int)buf[0], (int)buf[1]);
          int snr = driver.lastSNR();
          int rssi = driver.lastRssi();
          display.printf("%i <- RSSI %i SNR %i\n", MY_ADDRESS, rssi, snr);
          Serial.printf("%i, %i, -%i, %i, %i, %i\n", millis(), counter, (int)buf[0], (int)buf[1], rssi, snr);
        } else {
          display.println("No return reply");
        }
      } else {
        int retransmisison_count = manager.retransmissions();
        display.printf("%s sendtoWait failed %i retries\n", data, retransmisison_count);
        Serial.printf("%i, %i, sendtoWait Failed %i retries\n", millis(), counter,retransmisison_count);
        //DisplayUpperRight(Failure_Counter++);
      }
      DisplayUpperRight(counter);
      counter++;
    } //legal to transmit
  } // as a client
} //loop


void check_button() 
{
  #ifdef ARDUINO_LILYGO_T3_V1_6_1
    //We don't have a button at this time so just return
    return;
  #endif

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
      Serial.printf("modulation_index= %d\n");
      both.printf("Current Modulation %i %s\n", MY_CONFIG_NAME[modulation_index]);
      both.println("Double press button to change\n");
    } else {
      modulation_index = (modulation_index + 1) % MODULATION_INDEX_MAX;
      setModemConfig(modulation_index);
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