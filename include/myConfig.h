#ifndef MYCONFIG_H
#define MYCONFIG_H

//Template for creating per unit configuration

#define USE_WIFI   0    //Change to 1 to enable WIFI
#define WIFI_SSID "XXXXX"
#define WIFI_PASSWD "XZZZZZ"

//#define MYADDRESS   3   //Set your unit ID here

//This is temporary until the conversion to the "mesh" style
#define SERVER_ADDRESS 1 //Do not change
#ifdef ARDUINO_LILYGO_T3_V1_6_1  
#define MY_ADDRESS 1    //LILYGO BOARD server
#else
//#define MY_ADDRESS 1    //Raj Server
//#define MY_ADDRESS 2    //Ron, Fixed
#define MY_ADDRESS 3    //Keith, Fixed
//#define MY_ADDRESS 4    //Raj, Portable
//#define MY_ADDRESS 5    //Raj, experimentation
//#define MY_ADDRESS 6    //Raj, experimentation
//#define MY_ADDRESS 7    //Ron, portable
//#define MY_ADDRESS 8    //Keith, Portable
//#define MY_ADDRESS 9    //Spare
#endif


// Other configuration on a per unit basis will be here

#endif