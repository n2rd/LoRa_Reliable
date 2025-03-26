#ifndef GPS_H
#define GPS_H

#if (HAS_GPS == 1)

#include "TinyGPS++.h"
#include "HardwareSerial.h"
#include <ESP32Time.h>  //includes time.h, uses the RTC built into ESP32

#ifndef GPS_TIMEOUT
#define GPS_TIMEOUT 60  //time in seconds for gps acquisition
#endif

class GPSClass {
    public:
        typedef enum GPSPowerState {
            GPS_OFF = 0,
            GPS_TX = 1,
            GPS_ON = 2
        } PowerState;
        // Constructor
        GPSClass();
        // Function declarations
        //void initGPS();  // Function to initialize the GPS module
        void onoff(PowerState state);  // Function to turn on or off the GPS 
        PowerState onoffState();
        void setup();
        void loop();
        unsigned long getTimeStamp();
        bool getLocation(double *lat, double *lng, double *alt = NULL, double *hdop = NULL);
        char *latLonToMaidenhead(double latitude, double longitude, int precision);
        void maidenheadGridToLatLon(char* grid,double *lat, double *lon);
        const char *getPowerStateName(PowerState state);
        double distance(double lat1, double lon1, double lat2, double lon2);
        bool getLastLatLon(double *lat, double *lon);
        bool getRtcIsSet() { return rtcIsSet; }
        ESP32Time *getRtc() { return &rtc; }
        int getTimeDiff() { return timeDiff; }
        uint32_t getBaudRate() { return currentBaudRate; }
        void setBaudTestBufferPtr(char *baudTestBufferPtrArg) {baudTestBufferPtr = baudTestBufferPtrArg; }
        char* getBaudTestBufferPtr() { return baudTestBufferPtr; }
        void rtcTimeSetExternally() { rtcIsSet = true; }
        int baudSwitchReason;
        int baudTestBufferLen;
        TinyGPSPlus gps;
    private:
        //HardwareSerial GPSSerial;    //use Hardware UART1 for GPS
        uint32_t currentBaudRate;
        volatile PowerState powerState;
        char* baudTestBufferPtr;
        volatile bool rtcIsSet;
        double lastLat;
        double lastLon;
        int timeDiff;
        #ifdef ARDUINO_ARCH_ESP32
        static void GPSTask(void *pvParameter);
        #endif //ARDUINO_ARCH_ESP32
        const char *powerStateNames[3] = { "OFF", "TX", "ON"};
        static ESP32Time rtc;
};

extern GPSClass GPS;

#endif //(HAS_GPS ==1)
#endif //!defined(GPS_H)