#ifndef WM5500_H
#define WM5500_H

#include <WiFi.h>

#if defined(USE_WM5500_ETHERNET) && (USE_WM5500_ETHERNET == 1)
//Ethernet SPI
#define ETH_SPI_HOST SPI3_HOST
#define ETH_SPI_CLOCK_MHZ 25
#define ETH_INT 2
#define ETH_MISO 3
#define ETH_MOSI 40
#define ETH_SCK 39
#define ETH_CS 38
#define ETH_RST 4
#if 0
#define _ETHERNET_WEBSERVER_LOGLEVEL_       3
#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial
#include <WebServer_ESP32_W5500.h>
#endif
#include "TelnetStream2.h"

void WM5500_Setup();
#endif //defined(USE_WM5500_ETHERNET) && (USE_WM5500_ETHERNET == 1)
#endif //!defined(WM5500_H)