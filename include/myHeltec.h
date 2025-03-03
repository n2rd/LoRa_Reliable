/**
 * @file myHeltec.h
 * @brief Header file for the Heltec library, modified version of heltec_unofficial.h
 *
 * This is a modified version of heltec_unoffilial.h from https://github.com/ropg/heltec_esp32_lora_v3.git
 * 
 * This file contains the definitions and declarations for the Heltec library.
 * The library provides functions for controlling the Heltec ESP32 LoRa V3
 * board, including LED brightness control, voltage measurement, deep sleep
 * mode, and more.
 */

#ifndef myheltec_h
#define myheltec_h

#ifdef ARDUINO_LILYGO_T3_V1_6_1
#include "myLilyGoT3.h"
#else
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  #include "driver/temperature_sensor.h"
#else
  #include "driver/temp_sensor.h"
#endif

#include <RH_SX126x.h>
extern RH_SX126x driver;
#define DRIVER_TYPE RH_SX126x

// 'PRG' Button
#define BUTTON    GPIO_NUM_0

// LED pin & PWM parameters
#define LED_PIN   GPIO_NUM_35
#define LED_FREQ  5000
#define LED_CHAN  0
#define LED_RES   8

// External power control
#define VEXT      GPIO_NUM_36

// Battery voltage measurement
#define VBAT_CTRL GPIO_NUM_37
#define VBAT_ADC  GPIO_NUM_1
// SPI pins
#define SS        GPIO_NUM_8
#define MOSI      GPIO_NUM_10
#define MISO      GPIO_NUM_11
#define SCK       GPIO_NUM_9

// Radio pins
#define DIO1      GPIO_NUM_14
#define RST_LoRa  GPIO_NUM_12
#define BUSY_LoRa GPIO_NUM_13
// Display pins
#define SDA_OLED  GPIO_NUM_17
#define SCL_OLED  GPIO_NUM_18
#define RST_OLED  GPIO_NUM_21

// #ifdef HELTEC_NO_RADIOLIB
//   #define HELTEC_NO_RADIO_INSTANCE
// #else
//   #include "RadioLib.h"
//   // make sure the power off button works when using RADIOLIB_OR_HALT
//   // (See RadioLib_convenience.h)
//   #define RADIOLIB_DO_DURING_HALT heltec_delay(10)
//   #include "RadioLib_convenience.h"
// #endif

//needs library  https://github.com/ThingPulse/esp8266-oled-ssd1306.git
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

//needs library ropg/HotButton@^0.1.1
#include "HotButton.h"
extern HotButton button;

// #include <SPI.h>
// SPIClass* hspi = new SPIClass(HSPI);
// SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa, *hspi);


#define DISPLAY_GEOMETRY GEOMETRY_128_64
extern SSD1306Wire display;


/**
 * @brief Controls the VEXT pin to enable or disable external power.
 *
 * This function sets the VEXT pin as an output pin and sets its state based on
 * the given parameter. If the state is true, the VEXT pin is set to LOW to
 * enable external power. If the state is false, the VEXT pin is set to INPUT to
 * disable external power.
 *
 * @param state The state of the VEXT pin (true = enable, false = disable).
 */
void heltec_ve(bool state);
/**
 * @brief Measures the battery voltage.
 *
 * This function measures the battery voltage by controlling the VBAT_CTRL pin
 * and reading the analog value from the VBAT_ADC pin. The measured voltage is
 * then converted to a float value and returned.
 *
 * @return The battery voltage in volts.
 */
float heltec_vbat();
/**
 * @brief Controls the LED brightness based on the given percentage.
 *
 * This function sets up the LED channel, frequency, and resolution, and then
 * adjusts the LED brightness based on the given percentage. If the percentage
 * is 0 or less, the LED pin is set as an input pin.
 *
 * @param percent The brightness percentage of the LED (0-100).
 */
void heltec_led(int percent);
/**
 * @brief Puts the device into deep sleep mode.
 *
 * This function prepares the device for deep sleep mode by disconnecting from
 * WiFi, turning off the display, disabling external power, and turning off the
 * LED. It can also be configured to wake up after a certain number of seconds
 * using the optional parameter.
 *
 * @param seconds The number of seconds to sleep before waking up (default = 0).
 */
void heltec_deep_sleep(int seconds = 0);
/**
 * @brief Calculates the battery percentage based on the measured battery
 * voltage.
 *
 * This function calculates the battery percentage based on the measured battery
 * voltage. If the battery voltage is not provided as a parameter, it will be
 * measured using the heltec_vbat() function. The battery percentage is then
 * returned as an integer value.
 *
 * @param vbat The battery voltage in volts (default = -1).
 * @return The battery percentage (0-100).
 */
int heltec_battery_percent(float vbat = -1);
/**
 * @brief Checks if the device woke up from deep sleep due to button press.
 * 
 * @return True if the wake-up cause is a button press, false otherwise.
 */
bool heltec_wakeup_was_button();
/**
 * @brief Checks if the device woke up from deep sleep due to a timer.
 * 
 * This function checks if the device woke up from deep sleep due to a timer.
 * 
 * @return True if the wake-up cause is a timer interrupt, false otherwise.
 */
bool heltec_wakeup_was_timer();
/**
 * @brief Measures esp32 chip temperature
 * 
 * @return float with temperature in degrees celsius.
*/
float heltec_temperature();
/**
 * @brief No Description provided
 *
 * Presumbably powers on and off the display
 * * @param on display on true display off false
 */
void heltec_display_power(bool on);
/**
 * @brief The main loop function for the Heltec library.
 *
 * This function should be called in loop() of the Arduino sketch. It updates
 * the state of the power button and implements long-press power off if used.
 */
void heltec_loop();
/**
 * @brief Delays the execution of the program for the specified number of
 *        milliseconds.
 *
 * This function delays the execution of the program for the specified number of
 * milliseconds. During the delay, it also calls the heltec_loop() function to
 * allow for the power off button to be checked.
 *
 * @param ms The number of milliseconds to delay.
 */
void heltec_delay(int ms);

#define MODULATION_INDEX_MAX 9
bool setModemConfig(uint8_t index);
extern const char* MY_CONFIG_NAME[];

#define POWER_INDEX_MAX 7
extern float power[POWER_INDEX_MAX];
 
#define DRIVER_MAX_MESSAGE_LEN RH_SX126x_MAX_MESSAGE_LEN
#ifndef HAS_WIFI
#define HAS_WIFI  1 //DEFAULT IF not defined in myConfig.h
#endif //HAS_WIFI
#ifndef HAS_GPS
#define HAS_GPS 1 //DEFAULT IF not defined in myConfig.h
#endif //HAS_GPS
#if (HAS_GPS ==1)
#define GPS_ON_PIN GPIO_NUM_46
#define GPS_RX_PIN GPIO_NUM_47  //connected to GPS TX pad
#define GPS_TX_PIN GPIO_NUM_48  //connected to GPS RX pad
#endif //HAS_GPS==1

#ifdef HELTEC_POWER_BUTTON
    esp_sleep_enable_ext0_wakeup(BUTTON, LOW);
    button.waitForRelease();
#endif

#ifndef HAS_WM5500
#define HAS_WM5500 1
#endif //HAS_WM5500

#if HAS_WM5500
#define WM5500_MISO 3
#define WM5500_MOSI 40
#define WM5500_SCLK 39
#define WM5500_CS   38
#define WM5500_RST  4
#define WM5500_INT  2
#endif //HAS_WM5500

#endif //!defined(ARDUINO_LILYGO_T3_V1_6_1)
#endif //!defined(myheltec_h)
