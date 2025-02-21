#include "main.h"

uint16_t encode_grid4(String locator) {
    return ((locator[0] - 'A') * 18 + (locator[1] - 'A') )* 100 + (locator[2] - '0') * 10 + (locator[3] - '0') ;
  }
  
  void decode_grid4(uint16_t grid4, char *grid) {
    grid[0] = (grid4 / 180) + 'A';
    grid[1] = (grid4 % 180) / 10 + 'A';
    grid[2] = (grid4 % 100) / 10 + '0';
    grid[3] = (grid4 % 10) + '0';
    grid[4] = '\0';
  }