#include <Arduino.h>
#include "myLilyGoT3.h"
#ifdef ARDUINO_LILYGO_T3_V1_6_1

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, DISPLAY_GEOMETRY);
//HotButton button(BUTTON);
RH_RF95 driver(LORA_CS, LORA_DIO0);




#endif //defined(ARDUINO_LILYGO_T3_V1_6_1)