#ifndef MAIN_H
#define MAIN_H

#include "version.h"
#include <Arduino.h>
#include "myConfig.h"
#include <SPI.h>
#include <preferences.h>

#ifndef ARDUINO_LILYGO_T3_V1_6_1
#include  "myHeltec.h"
#else
#include "myLilyGoT3.h"
#endif

#include "parameters.h"
#include "PrintSplitter.h"
#include "OTA.h"
#include "telnet.h"
#include "cli.h"
#include "csv.h"
#include "clientServer.h"
#include "p2p.h"

#include <RHReliableDatagram.h>

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
#endif //USE_WIFI > 0

extern CsvClass csv_telnet;
extern CsvClass csv_serial;
extern PrintSplitter csv_both;
extern PrintSplitter ps_both;
extern PrintSplitter ps_all;
extern RHReliableDatagram manager;
extern void DisplayUpperRight(int count);
extern void check_button();
#endif //MAIN_H