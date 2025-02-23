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

#define MODULATION_INDEX_MAX 9
void setModemConfig(uint8_t index);
extern const char* MY_CONFIG_NAME[];

#define POWER_INDEX_MAX 7
extern float power[POWER_INDEX_MAX];

#define DRIVER_MAX_MESSAGE_LEN RH_RF95_MAX_MESSAGE_LEN

#define HAS_WIFI    1
#ifndef HAS_GPS
#define HAS_GPS     0  //Default if not in myConfig.h
#endif //HAS_GPS
#if (HAS_GPS ==1)
#define GPS_ON_PIN  1
#define GPS_RX_PIN  2
#define GPS_TX_PIN  3
#endif  //HAS_GPS==1

#endif //!defined(myLilyGoT3_h)