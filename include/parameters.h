#include <cstdint>
#include "RadioHead.h"
#include "RH_SX126x.h"

// the list of parameters
#define CALLSIGN 0
#define FREQUENCY_INDEX 1
#define GPS_STATE 2
#define TX_LOCK 3
#define SHORT_PAUSE 4
#define LAT_VALUE 5
#define LON_VALUE 6
#define GRID4 7
#define GRID5 8
#define GRID6 9
#define MODULATION_INDEX 10
#define POWER_INDEX 11
#define TX_INTERVAL 12
#define ADDRESS 13

// organized into a struct
typedef struct Parameters {
    char     callsign[10];    // (10B) 9 char max
    uint8_t  frequency_index;  // (1B) index 0 to 103 freq = 902.125 + 0.25 * index in MHz
    uint8_t  gps_state;     // (1B) 0 off, 1 on at tx, 2 on all the time
    uint8_t  tx_lock;       // (1B) 0 off, 1 on
    uint8_t  short_pause;   // (1B) 0 off, 1 on
    double   lat_value, lon_value;  // (16B) GPS location
    uint16_t grid4;  // (2B) 4 char grid square, encoded from 0 to 32,399
    char     grid5;  // (1B) 1 char subsquare identifier, encoded as an ascii char 
    char     grid6; // (1B) 1 char subsquare identifier, encoded as an ascii char 
    uint8_t  modulation_index; // (1B) index 0 to 8
    uint8_t  power_index; // (1B) index 0 to 6
    uint8_t  tx_interval; // (1B) 0 to 255 seconds
    uint8_t  address; // (1B) 0 to 255
} Parameters;

//for opening nvram preferences
#define RW_MODE false
#define RO_MODE true

//power
#define POWER_INDEX_MAX 7
float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};

#define MODULATION_INDEX_MAX 8
#define MODULATION_LABELS {"Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"}
#ifndef ARDUINO_LILYGO_T3_V1_6_1
static const RH_SX126x::ModemConfig MY_MODEM_CONFIG_TABLE[MODULATION_INDEX_MAX] =
{
    //  packetType, p1, p2, p3, p4, p5, p6, p7, p8
    // 0 Short Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_128, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 1 Short Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_256, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 2 Medium Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_512, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 3 Medium Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_1024, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 4 Long Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 5 Long Moderate
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 6 Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 7 Very Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_62_5, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
};

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

static const MYMODEM_CONFIG_TABLE TTGO_MODEM_CONFIG_TABLE[MODULATION_INDEX_MAX] = 
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

#define DRIVER_MAX_MESSAGE_LEN RH_RF95_MAX_MESSAGE_LEN
#endif

static const String modulation_labels[MODULATION_INDEX_MAX] = MODULATION_LABELS;

// Function declarations
void parameters_init();
float frequency_index_to_frequency(uint8_t index); 
