#include "myConfig.h"
#ifdef HAS_ENCODER // only include this file if we have encoder

#include "Arduino.h"
#include "Dialog_bolditalic_10.h"
#include "SSD1306Wire.h" // Include the necessary header file
#include "parameters.h"

#define MENU_DEBUG 0

//
// MENU Structure
//
#define MAX_MENUS 7
#define MAX_ITEMS 6

// menus
#define MAIN_MENU 0
#define POWER_MENU 1
#define MOD_MENU 2
#define ADD_MENU 3
#define FREQ_MENU 4
#define SET_MENU 5
#define EXIT_MENU 6

#define MENU_LABELS {"Main Menu", "Power Setting", "Modulation", "Frequency MHz", "Address", "Settings", "Exit Menu!"}

#define MENU_ITEM_LABELS {                                                          \
    {"Power", "Mod", "Address", "Freq", "Settings", "Exit Menu!"},                  \
    {"-9dBm", "-4dBm", "0dBm", "6dBm", "12dBm", "18dBm"},                           \
    {"Long Slow", "Long Med", "Long Fast", "Med Slow", "Short Slow", "Short Fast"}, \
    {"", "", "", "", "", ""},                                                       \
    {"", "", "", "", "", ""},                                                       \
    {"TX Lock", "Short PAUSE", "Write NV", "GPS off", "GPS at tx", "GPS on"},       \
    {"", "", "", "", "", ""}                                                        \
}

// scrolling goes from 0 to the following upper limits
#define MAX_ADDRESS 256         // address must be lower than this
#define MAX_FREQUENCY_INDEX 104 // 902.125 to 927.875 in 250Khz steps

#define DX 2 // x offset for highlight rectangle
#define DY 1 // y offset for highlight rectangle

#include "AiEsp32RotaryEncoder.h"

// Rotary encoder pins
#define RE_PIN_A GPIO_NUM_5
#define RE_PIN_B GPIO_NUM_6
#define RE_PIN_SW GPIO_NUM_7
#define RE_VCC_PIN -1

// Rotary encoder steps
#define RE_STEPS 4
#define ENABLE_SPEED 1

// Function declarations
void IRAM_ATTR readEncoderISR(); // Interrupt service routine for rotary encoder
void act_item_init(); // Identify menu items that are activated
void rotary_setup(); // Setup rotary encoder
void rotary_loop(); // Loop for rotary encoder
void menu(int m);
void highlight_item(int menu, int old_item, int new_item);
void on_button_short_click();
void on_button_long_click();
void handle_rotary_button();
void preferences_init();
void draw_regular_menu(int max_items);
void handle_rotary_button(); 
void IRAM_ATTR readEncoderISR(); 
void robot();
bool activate(int cmenu, int citem);
void write_add(int add);
void on_button_long_click();
void on_button_short_click();
void menu_setup();
void check_button(); //this is the user button on Heltec
void rotary_loop();
void menu(int m);
void highlight_item(int menu, int old_item, int new_item);
void on_button_short_click();
void on_button_long_click();
void robot();
void display_setup();

extern SSD1306Wire display;
// extern Heltec_ESP32 display;
// extern Preferences preferences;

#define POS_ARRAY {{4, 20}, {4, 35}, {4, 50}, {78, 20}, {78, 35}, {78, 50}}

struct Parameters
{
    char callsign[10];           // (10B) 9 char max
    uint8_t frequency_index;     // (1B) index 0 to 103 freq = 902.125 + 0.25 * index in MHz
    uint8_t gps_state;           // (1B) 0 off, 1 on at tx, 2 on all the time
    uint8_t tx_lock;             // (1B) 0 off, 1 on
    uint8_t short_pause;         // (1B) 0 off, 1 on
    double lat_value, lon_value; // (16B) GPS location
    uint16_t grid4;              // (2B) 4 char grid square, encoded from 0 to 32,399
    char grid5;                  // (1B) 1 char subsquare identifier, encoded as an ascii char
    char grid6;                  // (1B) 1 char subsquare identifier, encoded as an ascii char
    uint8_t modulation_index;    // (1B) index 0 to 8
    uint8_t power_index;         // (1B) index 0 to 6
    uint8_t tx_interval;         // (1B) 0 to 255 seconds
    uint8_t address;             // (1B) 0 to 255
} Parameters;

#endif // HAS_ENCODER