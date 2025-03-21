#ifndef MYCONFIG_H
#define MYCONFIG_H

//Template for creating per unit configuration

#define USE_WIFI   1    //Change to 1 to enable WIFI
#define WIFI_SSID "ssid"
#define WIFI_PASSWD "passwd"

//#define MYADDRESS   3   //Set your unit ID here

//This is temporary until the conversion to the "mesh" style
#define SERVER_ADDRESS 1 //Do not change
#ifdef ARDUINO_LILYGO_T3_V1_6_1  
#define MY_ADDRESS 1    //LILYGO BOARD server
#else
//#define MY_ADDRESS 1    //Raj Server
//#define MY_ADDRESS 2    //Ron, Fixed
#define MY_ADDRESS 3    //Keith, Fixed
//#define MY_ADDRESS 4    //Raj, Portable
//#define MY_ADDRESS 5    //Raj, experimentation
//#define MY_ADDRESS 6    //Raj, experimentation
//#define MY_ADDRESS 7    //Ron, portable
//#define MY_ADDRESS 8    //Keith, Portable
//#define MY_ADDRESS 9    //Spare
#endif
/*ADDITIOPNAL ADDRESS ALLOCATIONS:
    10-19   Raj
    20-29   Keith
    30-39   Ron
*/

#define DEFAULT_CALLSIGN    "xxxxxxx"
#define DEFAULT_FREQUENCY_INDEX    
#define DEFAULT_GPS_STATE    
#define DEFAULT_TX_LOCK    
#define DEFAULT_SHORT_PAUSE    
#define DEFAULT_LAT_VALUE    
#define DEFAULT_LON_VALUE    
#define DEFAULT_GRID4    
#define DEFAULT_GRID5    
#define DEFAULT_GRID6    
#define DEFAULT_MODULATION_INDEX    
#define DEFAULT_POWER_INDEX    
#define DEFAULT_TX_INTERVAL   
#define DEFAULT_ADDRESS
#define DEFAULT_PROMISCUOUS

//Diagnostic tools:
//#define USE_RANDOM_SIGREP_SLOT false                      //True = randomized slot selection
                                                            //False = deterministic slot selection for debugging
//#define DETERMINISTIC_SIGREP_SLOT_WIDTH 300               //ms per slot (this should really be a variable based on packet size...)
//#define DETERMINISTIC_SIGREP_MAX_RADIO_ADDRESS 30         //maximum number of radios while in deterministic mode
//#define DEFAULT_PROMISCUOUS                 0
//#define RH_PROMISCUOUS_MODE                 1     //Enable complation of promiscuous mode code

#define DEFAULT_GPS_BAUDRATE 9600 //M100 ublox is 115200
  
// Other configuration on a per unit basis will be here
//uncomment the folowing defines to override the defaults in main.cpp
//#define DEFAULT_FREQUENCY 905.2 //RF Frequency in Mhz
//#define DEFAULT_MODULATION_INDEX 5 //LongFast. see table in main.cpp to adjust
//#define DEFAULT_POWER_INDEX 4     //see power table in main.cpp, index 0 is -9dBm, index 6 is +22dBm max 

#define HAS_ENCODER 0
#if defined(HAS_ENCODER) && (HAS_ENCODER == 1)
    //
    // need to define ENCODER PINS HERE
    //
#endif

// #define HAS_GPS 0 //HAS_GPS deprecated no longer used
#define HAS_HARDWARE_ETHERNET_PORT 0
#if defined(HAS_HARDWARE_ETHERNET_PORT) && (HAS_HARDWARE_ETHERNET_PORT == 1)
    #define GPS_METHOD_SPI
    //#define GPS_METHOND_UART
#endif



#endif