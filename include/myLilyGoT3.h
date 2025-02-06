#ifndef myLilyGoT3_h
#define myLilyGoT3_h

//needs library  https://github.com/ThingPulse/esp8266-oled-ssd1306.git
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

#include <HotButton.h>

// I2c Display pins
#define SDA_OLED  GPIO_NUM_21
#define SCL_OLED  GPIO_NUM_22

#define DISPLAY_GEOMETRY GEOMETRY_128_64
extern SSD1306Wire display;
extern HotButton button;

#include <RH_RF95.h>
extern RH_RF95 driver;
#define DRIVER_TYPE RH_RF95

#define heltec_vbat() 0.0
#define heltec_battery_percent(X) 0
#define heltec_deep_sleep(X) esp_deep_sleep(0)


#endif //!defined(myLilyGoT3_h)