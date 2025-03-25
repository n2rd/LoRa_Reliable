#ifdef USE_WM5500_ETHERNET

#include "main.h"
#include "wm5500.h"
#ifndef _ETHERNET_WEBSERVER_LOGLEVEL_
#define _ETHERNET_WEBSERVER_LOGLEVEL_       3
#endif
#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial
#define WEBSERVER_H
#include <WebServer_ESP32_W5500.h>

#ifdef USE_WM5500_ETHERNET
  static TCPServer server(80);
#endif

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};


// Google DNS Server IP
//IPAddress myDNS(8, 8, 8, 8);

/*
 * W5500 "hardware" MAC address.
 */
uint8_t eth_mac[] = { 0xFE, 0xED, 0xDE, 0xAD, 0xBE, 0xEF };

/*
 * Define the static network settings for this gateway's ETHERNET connection
 * on your LAN.  These values must match YOUR SPECIFIC LAN.  The "eth_IP"
 * is the IP address for this gateway's ETHERNET port.
 */
#ifndef W5500_SUBNET
#define W5500_SUBNET  1
#endif

#ifndef W5500_UNIT
#define W5500_UNIT  146
#endif

#ifndef W5500_DNS
#define W5500_DNS 192, 168, 1, 1
#endif

IPAddress eth_ip(192, 168, W5500_SUBNET, W5500_UNIT);
IPAddress eth_mask(255, 255, 255, 0);		// Subnet mask.
IPAddress eth_dns1(W5500_DNS); 
IPAddress eth_dns2(8, 8, 8, 8); //Google
IPAddress eth_gw(192, 168, W5500_SUBNET, 1);		// *** CHANGE THIS to match YOUR Gateway (router).     ***

void WM5500_Setup()
{
  //now using esp32_w5500 webserver library by khoing
  // To be called before ETH.begin()
  ESP32_W5500_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  ETH.begin( ETH_MISO, ETH_MOSI, ETH_SCK, ETH_CS, ETH_INT, ETH_SPI_CLOCK_MHZ, ETH_SPI_HOST );
  ETH.config(eth_ip,eth_gw,eth_mask,eth_dns1,eth_dns2);
  //delay(10000);
  //ps_all.printf("W5500 Address: %s",ETH.localIP().toString());
  Serial.printf("W5500 Address: %s\r\n",ETH.localIP().toString());
}

#endif  //USE_WM5500_ETHERNET