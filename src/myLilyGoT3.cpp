#include <Arduino.h>
#include "myLilyGoT3.h"
#ifdef ARDUINO_LILYGO_T3_V1_6_1

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, DISPLAY_GEOMETRY);
//HotButton button(BUTTON);
RH_RF95 driver(LORA_CS, LORA_DIO0);


//LILLYGO_T3 RF95 aka SX127x chips
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

bool setModemConfig(uint8_t index) {
  if (index >= MODULATION_INDEX_MAX)
    index = MODULATION_INDEX_MAX - 1;
  driver.setSignalBandwidth(TTGO_MODEM_CONFIG_TABLE[index].bandwidth);
  driver.setSpreadingFactor(TTGO_MODEM_CONFIG_TABLE[index].spreadingFactor);
  driver.setCodingRate4(TTGO_MODEM_CONFIG_TABLE[index].codingRate4Denominator);
  return true;
}

const char* MY_CONFIG_NAME[MODULATION_INDEX_MAX] =
{
"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"
};

float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};


#endif //defined(ARDUINO_LILYGO_T3_V1_6_1)