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
BME280 myBMP280;
void scanI2CBus();
static bool isPresent = false;

bool bmp280isPresent()
{
  return bmp280isPresent;
}


void bmp280_setup()
{
  Wire1.begin(41U,42U,400000);
  //scanI2CBus();
  myBMP280.setI2CAddress(0x76);
  bool devicePresentResult = myBMP280.beginI2C(Wire1);
  //Serial.print("DevicePresentResult = "); Serial.println(devicePresentResult);
  if (devicePresentResult) {
    uint8_t  result = myBMP280.begin();
    if ( result == 0x58) //0x58 for bmp 0x60 for bme device
      Serial.println("BMP280 detected");
    else if ( result == 0x60) //0x58 for bmp 0x60 for bme device
      Serial.println("BME280 detected");
    else {
      Serial.println("No BMP280 or BME280 detected");
      isPresent = false;
      return;
    }
    isPresent = true;
    /*
    //myBMP280.setReferencePressure(101200); //Adjust the sea level pressure used for altitude calculations??
    BME280_SensorMeasurements measurements;
    myBMP280.readAllMeasurements(&measurements);
    //Serial.print("Humidity: ");
    //Serial.println(measurements.humidity);
    Serial.print("Pressure: ");
    Serial.println(measurements.pressure);
    Serial.print("Temperature: ");
    Serial.println(measurements.temperature);
    */
  }
  else {
    Serial.println("No BMP280 or BME280 detected");
  }
}
#if 0
#define WIRE Wire1
void scanI2CBus() {
  byte error, address;
  int nDevices;
  Serial.println("\nI2C Scanner");
  Serial.println("Scanning...");
  Serial.printf("%3d.",address);
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    if (address % 10 == 0)
        Serial.printf("\r\n%3d.",address);
    else
        Serial.print(".");
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
#endif //0
