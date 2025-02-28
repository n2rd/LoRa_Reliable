#include "main.h"

  uint16_t encode_grid4(char *locator) {
    return ((locator[0] - 'A') * 18 + (locator[1] - 'A') )* 100 + (locator[2] - '0') * 10 + (locator[3] - '0') ;
  }
  
  void encode_grid4_to_buffer(char *locator, uint8_t* buffer) {
    uint16_t wordValue = ((locator[0] - 'A') * 18 + (locator[1] - 'A') )* 100 + (locator[2] - '0') * 10 + (locator[3] - '0') ;
    buffer[0] = (wordValue & 0xFF00) >> 8; //HiByte first
    buffer[1] = wordValue & 0xFF; //Low Byte second
  }
  
  void decode_grid4(uint16_t grid4, char *grid) {
    grid[0] = (grid4 / 1800) + 'A';
    grid[1] = (grid4 % 1800) / 100 + 'A';
    grid[2] = (grid4 % 100) / 10 + '0';
    grid[3] = (grid4 % 10) + '0';
    grid[4] = '\0';
  }

  void decode_grid4_from_buffer(uint8_t* buffer, char *grid)
  {
    uint16_t word = (buffer[0] << 8) | buffer[1];
    decode_grid4(word, grid);
  }