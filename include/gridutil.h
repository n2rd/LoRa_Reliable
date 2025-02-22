#ifndef GRIDUTIL_H
#define GRIDUTIL_H

#include "TinyGPS++.h"
#include "HardwareSerial.h"

uint16_t    encode_grid4(String locator);
void        decode_grid4(uint16_t grid4, char *grid);

#endif //GRIDUTIL_H