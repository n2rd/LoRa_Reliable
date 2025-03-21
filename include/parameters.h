#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "main.h"

//#include <cstdint>
//#include "RadioHead.h"
//#include "RH_SX126x.h"

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
#define PROMISCUOUS 14

//array sizes
#define FREQUENCY_INDEX_MAX 103  //0 is 902.125, 103 is 927.875
#define ADDRESS_MAX 254  //255 is broadcast 
#define MENU_ARRAY_SIZE 6  //used for power, modulation, settings menus

//DEFAULT_XXX values. These are overrideable in myConfig.h by defining them
#ifndef DEFAULT_CALLSIGN
#define DEFAULT_CALLSIGN    "----"
#endif
#ifndef DEFAULT_FREQUENCY_INDEX
#define DEFAULT_FREQUENCY_INDEX 12
#endif
#ifndef DEFAULT_GPS_STATE    
#define DEFAULT_GPS_STATE 0
#endif
#ifndef DEFAULT_TX_LOCK  
#define DEFAULT_TX_LOCK 0
#endif
#ifndef DEFAULT_SHORT_PAUSE
#define DEFAULT_SHORT_PAUSE 0
#endif
#ifndef DEFAULT_LAT_VALUE
#define DEFAULT_LAT_VALUE 43.01
#endif
#ifndef DEFAULT_LON_VALUE   
#define DEFAULT_LON_VALUE -76.3
#endif
#ifndef DEFAULT_GRID4   
#define DEFAULT_GRID4 10313 //FN13 grid 4
#endif
#ifndef DEFAULT_GRID5  
#define DEFAULT_GRID5 'u'
#endif
#ifndef DEFAULT_GRID6   
#define DEFAULT_GRID6 'a'
#endif
#ifndef DEFAULT_MODULATION_INDEX   
#define DEFAULT_MODULATION_INDEX 5
#endif
#ifndef DEFAULT_POWER_INDEX   
#define DEFAULT_POWER_INDEX 4
#endif
#ifndef DEFAULT_TX_INTERVAL   
#define DEFAULT_TX_INTERVAL 30
#endif
#ifndef DEFAULT_ADDRESS
#define DEFAULT_ADDRESS 4   
#endif
#ifndef DEFAULT_PROMISCUOUS
#define DEFAULT_PROMISCUOUS 0
#endif
#ifndef USE_RANDOM_SIGREP_SLOT
#define USE_RANDOM_SIGREP_SLOT true
#endif
#ifndef DETERMINISTIC_SIGREP_SLOT_WIDTH
#define DETERMINISTIC_SIGREP_SLOT_WIDTH 300 //ms per slot
#endif
#ifndef DETERMINISTIC_SIGREP_MAX_RADIO_ADDRESS
#define DETERMINISTIC_SIGREP_MAX_RADIO_ADDRESS 30 //maximum number of radios
#endif

// organized into a struct
typedef struct ParametersStruct {
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
    uint8_t  address; // (1B) 0 to 254 (255=broadcast address)
    uint8_t  radioType; //Radio Type ... the meaning of the values: 0,1,2 needs to be clarified
    char     wifiSSID[33]; //32+null
    char     wifiKey[63+1]; //+ null;
    bool     serialCSVEnabled;
    bool     telnetCSVEnabled;
    uint8_t  promiscuousEnabled;
} PARAMETERS;

//for opening nvram preferences
#define RW_MODE false
#define RO_MODE true 

class ParametersClass {
  public:
    ParametersClass() { init(); }
    float frequency_index_to_frequency(uint8_t index);
    uint8_t frequency_to_frequency_index(float frequency); 
    bool set_frequency();  //based on frequency_index
    //bool set_callsign();
    //bool set_gps_state();
    //bool set_tx_lock();
    //bool set_short_pause();
    //bool set_lat_lon();
    //bool set_grid4();
    //bool set_grid5();
    //bool set_grid6();
    bool set_power();
    bool set_modulation();
    void set_address();
    void update(); 

    PARAMETERS parameters; //parameters for the radio

    struct keyStruct {
      const char* callsign = "callsign";
      const char* frequency_index = "frequency_index";
      const char* gps_state = "gps_state";
      const char* tx_lock = "tx_lock";
      const char* short_pause = "short_pause";
      const char* lat_value = "lat_value";
      const char* lon_value = "lon_value";
      const char* grid4 = "grid4";
      const char* grid5 = "grid5";
      const char* grid6 = "grid6";
      const char* modulation_index = "mod_index";
      const char* power_index = "power_index";
      const char* tx_interval = "tx_interval";
      const char* address = "address";
      const char* radioType = "radioType";
      const char* wifiSSID = "WifiSSID";
      const char* wifiKey = "WifiKey";
      const char* serialCSVEnabled = "SerCSVEnabled";
      const char* telnetCSVEnabled = "TelCSVEnabled";
      const char* promiscuousEnabled = "PromisEnabled";
    } Key;
  private:
    static Preferences preferences;
    void init();
    size_t putString(const char *key, char *value);
    size_t putUInt8(const char *key, uint8_t byte );
    size_t putUInt16(const char *key, uint16_t word );
    size_t putChar(const char *key, char value);
    size_t putFloat(const char *key, float value);
    void DumpKeyUint(char *keyToDump);
    void DumpKeyString(char *keyToDump);
};

extern ParametersClass PARMS;

#endif //PARAMETERS_H