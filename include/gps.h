//includes for gps.cpp
#include "TinyGPS++.h"
#include "HardwareSerial.h"

// GPS pin definitions
#define GPS_ON_PIN GPIO_NUM_46
#define GPS_RX_PIN GPIO_NUM_47  //connected to GPS TX pad
#define GPS_TX_PIN GPIO_NUM_48  //connected to GPS RX pad

#ifndef GPS_TIMEOUT
#define GPS_TIMEOUT 60  //time in seconds for gps acquisition
#endif

//GPS modes of usage
#define GPS_OFF 0
#define GPS_TX 1
#define GPS_ON 2

// Function declarations
String latLonToMaidenhead(double latitude, double longitude, int precision);
void initGPS();  // Function to initialize the GPS module
void gps_toggle(int state);  // Function to turn on or off the GPS 
uint16_t encode_grid4(String locator); // Function to encode a 4 character grid square into a 16 bit integer
void decode_grid4(uint16_t grid4, char *grid); // Function to decode a 16 bit integer into a 4 character grid square 

