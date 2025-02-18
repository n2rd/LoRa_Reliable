#ifndef MYCONFIG_H
#define MYCONFIG_H

// Template for creating per unit configuration

#define USE_WIFI 1 // Change to 1 to enable WIFI
#define WIFI_SSID "dewan"
#define WIFI_PASSWD "cheerful"

//
// defaults for parameters in mypreferences.h
//
// LoRa settings that are used for Meshtastic
//
// No Channel Name	 / Data-Rate	/ SF/ Symb	/ Coding Rate	/ Bandwidth	/ Link Budget
// 0  Short Turbo	   / 21.88 kbps /	7 / 128	  / 4/5	        / 500 kHz	  / 140dB
// 1  Short Fast	   / 10.94 kbps / 7 / 128	  / 4/5	        / 250 kHz	  / 143dB
// 2  Short Slow	   / 6.25 kbps	/ 8 / 256	  / 4/5	        / 250 kHz	  / 145.5dB
// 3  Medium Fast	   / 3.52 kbps	/ 9 / 512	  / 4/5	        / 250 kHz	  / 148dB
// 4  Medium Slow	   / 1.95 kbps	/ 10 / 1024	/ 4/5	        / 250 kHz	  / 150.5dB
// 5  Long Fast	     / 1.07 kbps	/ 11 / 2048	/ 4/5	        / 250 kHz	  / 153dB
// 6  Long Moderate  / 0.34 kbps	/ 11 / 2048	/ 4/8	        / 125 kHz	  / 156dB
// 7  Long Slow	     / 0.18 kbps	/ 12 / 4096	/ 4/8	        / 125 kHz	  / 158.5dB
// 8  Very Long Slow / 0.09 kbps	/ 12 / 4096 /	4/8	        / 62.5 kHz	/ 161.5dB
//
// Frequency = 902.125 + 0.25 * index in MHz as in Meshatastic
//
// Power index 0 is -9dBm, index 6 is +22dBm max
// power values {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0} dBm


#define DEFAULT_CALLSIGN "N2RD"
#define DEFAULT_FREQUENCY_INDEX 12 // 905.125 = 902.125 + 0.25 * index in MHz
#define DEFAULT_GPS_STATE 0        // 0 off, 1 on at tx, 2 on all the time
#define DEFAULT_TX_LOCK 0          // 0 off, 1 on
#define DEFAULT_SHORT_PAUSE 0      // 0 off, 1 on
#define DEFAULT_LAT_VALUE 43.01    // Syracuse NY
#define DEFAULT_LON_VALUE -76.3
#define DEFAULT_GRID4 10313 // (FN13) grid4
#define DEFAULT_GRID5 'u'
#define DEFAULT_GRID6 'a'
#define DEFAULT_MODULATION_INDEX 5 // see LoRa settings table below
#define DEFAULT_POWER_INDEX 4 // see power table in main.cpp, index 0 is -9dBm, index 6 is +22dBm max
#define DEFAULT_TX_INTERVAL 30 // 0 to 255 seconds
#define DEFAULT_ADDRESS 4 // 2 to 255 for client or peer to peer, use 1 for server

#define SERVER_ADDRESS 1 // Do not change

#define HAS_ENCODER 1
#if defined(HAS_ENCODER) && (HAS_ENCODER == 1)
#include "menu.h" //for rotary encoder and menu
#endif

#define HAS_GPS 1
#ifdef HAS_GPS
#include "gps.h"
#endif

#define HAS_HARDWARE_ETHERNET_PORT 0
#if defined(HAS_HARDWARE_ETHERNET_PORT) && (HAS_HARDWARE_ETHERNET_PORT == 1)
#define GPS_METHOD_SPI
// #define GPS_METHOND_UART
#endif

#endif