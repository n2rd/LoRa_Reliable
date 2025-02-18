#ifndef MAIN_H
#define MAIN_H
// include standard libs from Espressif ESP32 IDF first
#include <Arduino.h>
#include "HardwareSerial.h"
#include "Preferences.h"

//includes from libraries
#include <RHReliableDatagram.h>
#include <SPI.h>
#include "OTA.h"
#include "telnet.h"

//includes from this project
#include "version.h"
#include "myConfig.h"
#include "PrintSplitter.h"
#include "cli.h"
#include "csv.h"
#include "parameters.h"

//conditional includes, definitions and declarations
#ifndef ARDUINO_LILYGO_T3_V1_6_1
  // Turns the 'PRG' button into the power button, long press is off 
  #define HELTEC_DEFAULT_POWER_BUTTON   // must be before #include myHeltec.h"
  #include "myHeltec.h"
#else
#include "myLilyGoT3.h"
#endif

#if USE_WIFI >0
    #if defined(ESP32)
    #include <WiFi.h>
    #include <WiFiClient.h>
    #include <ElegantOTA.h>
        #if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
        #include <ESPAsyncWebServer.h>
        extern AsyncWebServer server;
        #else
        #include <WebServer.h>
        extern WebServer server;
        #endif
    #endif //defined(ESP32)
#endif //USE_WIFI > 0

//
// definitions and declarations
//

// Pause between transmited packets in seconds.
#define PAUSE       20  // client, time between transmissions in seconds

// Timeout for sendtoWait in milliseconds
#define TIMEOUT     200  //for sendtoWait
#define RETRIES     3     //for sendtoWait
#ifdef HAS_GPS
//#define GPS_TIMEOUT 60  //time in seconds for gps acquisition
//function declarations for gps.cpp
void gps_setup();
bool get_gps_location(double &lat, double &lng, double &alt, double &hdop); 
uint16_t encode_grid4(String locator); 
void decode_grid4(uint16_t grid4, char *grid); 
#endif

extern PrintSplitter both;
//
// functions defined in main.cpp
//
void check_button();
void setup_radio_parameter(int item, int index);
void DisplayUpperRight(int count); 
void toggleLED();

#endif //MAIN_H