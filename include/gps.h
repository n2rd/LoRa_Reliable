#ifndef GPS_H
#define GPS_H

#if defined(HAS_GPS) && (HAS_GPS ==1)

#include "TinyGPS++.h"
#include "HardwareSerial.h"



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
/*
CODE MOVED TO GRIDUTIL
uint16_t encode_grid4(String locator); // Function to encode a 4 character grid square into a 16 bit integer
void decode_grid4(uint16_t grid4, char *grid); // Function to decode a 16 bit integer into a 4 character grid square 
*/
#else 
// stubs go here

#endif //#if defined(HAS_GPS) && (HAS_GPS ==1)
#endif //!defined(GPS_H)