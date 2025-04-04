#include "main.h"

//Convert millis to string of format dddd-hh:mm-ss: length =14 (13 + 1 for null)

void milliToDhms(char* returnString, int millis) {
    uint8_t hours, minutes, seconds;
    int days, remainder;
    
    remainder = millis/1000;
    seconds   = remainder%60;
    remainder = remainder/60;
    minutes   = remainder%60;
    remainder = remainder/60;
    hours     = remainder%24;
    remainder = remainder/24;
    days      = remainder;

    sprintf(returnString, "%04u-%02u:%02u:%02u", days, hours, minutes, seconds);
    
    return;
}


