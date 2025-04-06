#ifndef BMP280SENSOR_H
#define BMP280SENSOR_H
#include <Wire.h>
#include "Adafruit_BMP280.h"

void scanI2CBus(Print& printDev, TwoWire& WIRE);

class BMP280Sensor
{
    public:
        BMP280Sensor() : bIsPresent(false) { myBMP280 = Adafruit_BMP280(&Wire1); }
        bool setup();
        bool isPresent();
        float readTempF();
        float readTempC();
        float readPressurePa();
    private:
        bool bIsPresent;
        Adafruit_BMP280 myBMP280;
};

extern BMP280Sensor BMP280;

#endif //BMP280SENSOR_H