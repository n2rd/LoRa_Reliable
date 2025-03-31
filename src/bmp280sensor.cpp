/*
  Adjust the local Reference Pressure
  Nathan Seidle @ SparkFun Electronics
  March 23, 2018

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  'Sea level' pressure changes with high and low pressure weather movement. 
  This sketch demonstrates how to change sea level 101325Pa to a different value.
  See Issue 1: https://github.com/sparkfun/SparkFun_BME280_Arduino_Library/issues/1

  Google 'sea level pressure map' for more information:
  http://weather.unisys.com/surface/sfc_con.php?image=pr&inv=0&t=cur
  https://www.atmos.illinois.edu/weather/tree/viewer.pl?launch/sfcslp

  29.92 inHg = 1.0 atm = 101325 Pa = 1013.25 mb
*/
#include "main.h"
#include <Wire.h>
#include "SparkFunBME280.h"

BMP280Sensor BMP280;

bool BMP280Sensor::isPresent()
{
  return bIsPresent;
}

bool BMP280Sensor::setup()
{
  //scanI2CBus();
  bIsPresent = false;
  myBMP280.setI2CAddress(0x76);
  bool devicePresentResult = myBMP280.beginI2C(Wire1);
  log_v("DevicePresentResult = %d",devicePresentResult);
  if (devicePresentResult) {
    uint8_t  result = myBMP280.begin();
    if ( result == 0x58) //0x58 for bmp 0x60 for bme device
      log_v("BMP280 detected");
    else if ( result == 0x60) //0x58 for bmp 0x60 for bme device
      log_v("BME280 detected");
    else {
      log_d("No BMP280 or BME280 detected");
      return false;
    }
    bIsPresent = true;
    return true;
  }
  else {
    log_d("No BMP280 or BME280 detected");
    return false;
  }
}

void scanI2CBus(Print& printDev, TwoWire& WIRE) {
  byte error, address;
  int nDevices;
  printDev.printf("\r\nI2C Scanner\r\n");
  printDev.printf("Scanning 127 addresses ...\r\n");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    if (address % 10 == 0)
        printDev.printf("\r\n%3d.",address);
    else
        printDev.print(".");
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();
    if (error == 0)
    {
      printDev.printf("I2C device found at address %02X\r\n", address);
      nDevices++;
    }
    else if (error==4)
    {
      printDev.printf("Unknown error at address %02X", address);
    }
  }
  if (nDevices == 0)
    printDev.printf("No I2C devices found\r\n");
  else
    printDev.printf("done\n");
}
