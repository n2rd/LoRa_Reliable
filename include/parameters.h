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
    uint8_t  address; // (1B) 0 to 255
} PARAMETERS;

//for opening nvram preferences
#define RW_MODE false
#define RO_MODE true 

# warning "The next 2 lines don't seem to be used"
//#define MODULATION_LABELS {"Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"}
//static const String modulation_labels[MODULATION_INDEX_MAX+1] = MODULATION_LABELS;
class ParametersClass {
  public:
    ParametersClass() { init(); }
    float frequency_index_to_frequency(uint8_t index);
    uint8_t frequency_to_frequency_index(float frequency); 

    PARAMETERS parameters; //parameters for the radio

    size_t putString(const char *key, char *value);
    size_t putUInt8(const char *key, uint8_t byte );
    size_t putUInt16(const char *key, uint16_t word );
    size_t putChar(const char *key, char value);
    size_t putFloat(const char *key, float value);
  private:
    Preferences preferences;
    void init();
};

extern ParametersClass PARMS;

#endif //PARAMETERS_H