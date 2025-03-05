#ifndef BMP280SENSOR_H
#define BMP280SENSOR_H

#include "SparkFunBME280.h"
bool bmp280_isPresent();
bool bmp280_setup();

extern BME280 myBMP280;

#endif //BMP280SENSOR_H