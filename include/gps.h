#ifndef GPS_H
#define GPS_H

#if (HAS_GPS == 1)

#include "TinyGPS++.h"
#include "HardwareSerial.h"

#ifndef GPS_TIMEOUT
#define GPS_TIMEOUT 60  //time in seconds for gps acquisition
#endif

class GPSClass {
    public:
        enum GPSstate {
            GPS_OFF = 0,
            GPS_TX = 1,
            GPS_ON = 2
        } GPSSTATE;
        typedef enum GPSstate State;
        // Constructor
        GPSClass();
        // Function declarations
        //void initGPS();  // Function to initialize the GPS module
        void onoff(State state);  // Function to turn on or off the GPS 
        void setup();
        bool getLocation(double *lat, double *lng, double *alt = NULL, double *hdop = NULL);
        String latLonToMaidenhead(double latitude, double longitude, int precision);
    private:
        //HardwareSerial GPSSerial;    //use Hardware UART1 for GPS
        TinyGPSPlus gps;
};

extern GPSClass GPS;

#endif //(HAS_GPS ==1)
#endif //!defined(GPS_H)