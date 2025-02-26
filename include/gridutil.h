#ifndef GRIDUTIL_H
#define GRIDUTIL_H

#include "TinyGPS++.h"
#include "HardwareSerial.h"

uint16_t    encode_grid4(char* locator);
void        decode_grid4(uint16_t grid4, char *grid);
void        encode_grid4_to_buffer(char *locator, uint8_t* buffer);
void        decode_grid4_from_buffer(uint8_t* buffer, char *grid);

#endif //GRIDUTIL_H