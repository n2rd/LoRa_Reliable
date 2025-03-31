#ifndef BMP280SENSOR_H
#define BMP280SENSOR_H

#include "SparkFunBME280.h"

void scanI2CBus(Print& printDev, TwoWire& WIRE);

class BMP280Sensor
{
    public:
        BMP280Sensor() : bIsPresent(false) {}
        bool setup();
        bool isPresent();
        float readTempF() { return myBMP280.readTempF(); }
        float readTempC() { return myBMP280.readTempC(); }
    private:

        bool bIsPresent;
        BME280 myBMP280;
};

extern BMP280Sensor BMP280;

#endif //BMP280SENSOR_H