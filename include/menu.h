#include "main.h"
#if defined(HAS_ENCODER) && HAS_ENCODER == 1// only include this file if we have encoder

#include "Dialog_bolditalic_10.h"

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
void rolling_menu(int r_menu_num);
void show_rolling_item(int r_menu_num, int position, int item_num);
void draw_regular_menu(int menu, int max_items);

extern SSD1306Wire display;
// extern Heltec_ESP32 display;
// extern Preferences preferences;
extern bool menu_active;
#define POS_ARRAY {{4, 20}, {4, 35}, {4, 50}, {78, 20}, {78, 35}, {78, 50}}

#endif // HAS_ENCODER