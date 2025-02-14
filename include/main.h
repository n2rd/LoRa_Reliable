#ifndef MAIN_H
#define MAIN_H
#include "version.h"
#include <Arduino.h>
#include "myConfig.h"
#include <SPI.h>
#include "PrintSplitter.h"
#include "OTA.h"
#include "telnet.h"
#include "preferences.h"
#include "cli.h"

#ifndef ARDUINO_LILYGO_T3_V1_6_1
#include  "myHeltec.h"
#else
#include "myLilyGoT3.h"
#endif

#include <RHReliableDatagram.h>

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

extern PrintSplitter both;

#endif //MAIN_H