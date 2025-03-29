#ifndef MAIN_H
#define MAIN_H

#include "myConfig.h"
#include "version.h"
#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include <Preferences.h>
#include <RHDatagram.h>

#ifndef ARDUINO_LILYGO_T3_V1_6_1
#include  "myHeltec.h"
#else
#include "myLilyGoT3.h"
#endif

#include "parameters.h"
#include "PrintSplitter.h"

#if USE_OTA >0
#include "OTA.h"
#endif

#if USE_TELNET >0
#include "telnet.h"
#endif

#include "cli.h"
#include "csv.h"
#include "gridutil.h"
#include "clientServer.h"
#include "p2p.h"
#include "gps.h"
#include "menu.h"
#include "bmp280sensor.h"

#if defined(USE_WM5500_ETHERNET) && (USE_WM5500_ETHERNET == 1)
#include "wm5500.h"
#endif

#ifdef HAS_INA219
#include "inaSensor.h"
#endif //has_ina219

/*
#if USE_WIFI >0
    #if defined(ESP32)
        #if defined(HAS_WIFI) && (HAS_WIFI == 1)
            #include <WiFi.h>
            #include <WiFiClient.h>
            #include <ElegantOTA.h>
            #if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && (ELEGANTOTA_USE_ASYNC_WEBSERVER == 1)
            #include <ESPAsyncWebServer.h>
            extern AsyncWebServer server;
            #else //!(defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1)
            #include <WebServer.h>
            extern WebServer server;
            #endif //!(defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1)
        #endif //defined(HAS_WIFI) && (HAS_WIFI == 1)
    #endif //defined(ESP32)
    #include "wifiX.h"
#endif //USE_WIFI > 0
*/
#if defined(ESP32)
    #if defined(HAS_WIFI) && (HAS_WIFI == 1)
        #include <WiFi.h>
        #include <WiFiClient.h>
        #if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && (ELEGANTOTA_USE_ASYNC_WEBSERVER == 1)
            #include <ElegantOTA.h>
            #include <ESPAsyncWebServer.h>
        #endif //!(defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1)
    #endif //defined(HAS_WIFI) && (HAS_WIFI == 1)
#endif //defined(ESP32)
#include "wifiX.h"

#define DECLARE_MUTEX(X) pthread_mutex_t X;						   
#define MUTEX_INIT(X) pthread_mutex_init(&X, NULL)
#define MUTEX_LOCK(X) pthread_mutex_lock(&X)
#define MUTEX_TRYLOCK(X) pthread_mutex_trylock(&X)
#define MUTEX_UNLOCK(X) pthread_mutex_unlock(&X)

extern CsvClass csv_telnet;
extern CsvClass csv_serial;
extern PrintSplitter csv_both;
extern PrintSplitter ps_both;
extern PrintSplitter ps_st;
extern PrintSplitter ps_all;
extern RHDatagram manager;
extern void DisplayUpperRight(int count);
extern void check_button();

//
// functions declarations of fucntions in main.cpp 
//
void initializeNetwork();
void DisplayUpperRight(int count);
void toggleLED();
#if HAS_GPS
void dumpLatLon();
#endif //HAS_GPS

#ifdef DUMP_PARTITIONS
void dumpPartitions(); 
#endif //DUMP_PARTITIONS

#endif //MAIN_H
