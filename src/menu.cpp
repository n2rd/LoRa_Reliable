#include "main.h"

bool menu_active = false;  //do not show radio messages on display when menu is active

#if defined(HAS_ENCODER) && HAS_ENCODER == 1  //only include this file if we have encoder

#include "AiEsp32RotaryEncoder.h"
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(RE_PIN_A, RE_PIN_B, RE_PIN_SW, RE_VCC_PIN, RE_STEPS);

//paramaters for button
unsigned long shortPressAfterMiliseconds = 100;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 1000;  //how long čong press shoud be.


extern bool menu_active;  //do not show radio messages on display when menu is active

int cur_menu = 0;
int act_item[MAX_MENUS] = {0, 0, 0, 0, 0, 0, 0};  //this has been activated
int cur_item[MAX_MENUS] = {0, 0, 0, 0, 0, 0, 0};  //this has been selected

String menu_label[MAX_MENUS] = MENU_LABELS;
String item_label[MAX_MENUS][MAX_ITEMS] = MENU_ITEM_LABELS;

//Menu Item display configuration
// 128 x 64 display divided into two columns
// The coordinates define the left starting point of the text
int pos[MAX_ITEMS][2] = POS_ARRAY;

void IRAM_ATTR readEncoderISR()
{
  rotary.readEncoder_ISR();
}
  
//identify menu items that are activated
// these will be shown in bold italic type
void act_item_init()
{
  act_item[0] = -1;  //nothing activated on main menu yet
  act_item[1] = PARMS.parameters.power_index;
  if (MENU_DEBUG) Serial.printf("act_item_init: power %i\n", act_item[1]);
  act_item[2] = PARMS.parameters.modulation_index;
  if (MENU_DEBUG) Serial.printf("act_item_init: modulation %i\n", act_item[2]);
  act_item[3] = PARMS.parameters.address;
  if (MENU_DEBUG) Serial.printf("act_item_init: address %i\n", act_item[3]);
  act_item[4] = PARMS.parameters.frequency_index;
  if (MENU_DEBUG) Serial.printf("act_item_init: frequency %i\n", act_item[4]);
  act_item[5] = PARMS.parameters.gps_state;
  if (MENU_DEBUG) Serial.printf("act_item_init: gps %i\n", act_item[5]);
  tx_lock = PARMS.parameters.tx_lock;
  if (MENU_DEBUG) Serial.printf("act_item_init: tx_lock %i\n", tx_lock);
  short_pause = PARMS.parameters.short_pause;
  if (MENU_DEBUG) Serial.printf("act_item_init: short_pause %i\n", short_pause);
}

void rotary_setup()
{
  //rotary setup
  pinMode(RE_PIN_A, INPUT_PULLUP);
  pinMode(RE_PIN_B, INPUT_PULLUP);
  pinMode(RE_PIN_SW, INPUT_PULLUP);

  //we must initialize rotary encoder
  rotary.begin();
  rotary.setup(readEncoderISR);
  //set boundaries and if values should cycle or not
  bool circleValues = true;
  rotary.setBoundaries(0, MAX_ITEMS-1, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotary.disableAcceleration(); //disable acceleration 
  //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}
  
void rotary_loop()
{
  // robot for menu demo
  //`robot();

  //dont print anything unless value changed
  if (rotary.encoderChanged()) {
	  int new_item = rotary.readEncoder();
	  if (MENU_DEBUG) Serial.printf("encoder changed %i\n", new_item);
	  if (cur_menu != ADD_MENU && cur_menu != FREQ_MENU) {  //if regular menu
	    menu(cur_menu);
	    highlight_item(cur_menu, cur_item[cur_menu], new_item);
	    cur_item[cur_menu] = new_item;
	  } else {
      cur_item[cur_menu] = new_item;
      rolling_menu(cur_menu);
	  }
  }
  handle_rotary_button();
}

//implements the rolling menu that is used for
// menus that have more than 6 items such as the frequency and address menus
// it show three rows of items, the current item in the middle
// the three rows are identified by position -1, 0, 1
// the indexes for items roll in a circle in both directions 
// going from zero and maximum value and back
void rolling_menu(int r_menu_num)
{  
  display.clear();
  cur_menu = r_menu_num;
  //menu header
  display.setFont(ArialMT_Plain_16);
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, menu_label[r_menu_num]);

  //first determine the upper limit
  int max_index = 0;
  if (cur_menu = ADD_MENU) {
    max_index = MAX_ADDRESS;
  } else if (cur_menu = FREQ_MENU) { 
    max_index = MAX_FREQUENCY_INDEX;
  }

  // -1 position
  if (cur_item[cur_menu] > 0) {
    show_rolling_item(r_menu_num, -1, cur_item[cur_menu]-1);
  } else {
    show_rolling_item(r_menu_num, -1, max_index);
  }

  // 0 position
  show_rolling_item(r_menu_num, 0, cur_item[cur_menu]);

  // +1 position
  if (cur_item[cur_menu] == max_index) {
    show_rolling_item(r_menu_num, 1, 0);
  } else {
    show_rolling_item(r_menu_num, 1, cur_item[cur_menu] + 1);
  }
  display.display();
}

//shows the rolling menu item
// puts the current item in a highlight box
// activated item is shown in bold italic font
void show_rolling_item(int r_menu_num, int position, int item_num) //position -1, 0, 1
{
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  int y = 32 + position * 15;
  if (item_num == act_item[cur_menu]) {
    display.setFont(Dialog_bolditalic_10); 
  } else {
    display.setFont(ArialMT_Plain_10);
  }
  char item_str[10];
  if (r_menu_num == ADD_MENU) {
    sprintf(item_str, "%i", item_num);
  } else if (r_menu_num == FREQ_MENU) {
    sprintf(item_str, "%i", 902125 + 250 * item_num);
  }
  display.drawString(64, y, item_str);
  // if it is the center one, highlight it
  if (position == 0) {
    display.drawRect(48, 33, 32, 12);
  }
}

//draws the menu on the screen
// used the regular menu structure for non-rolling menus
//   this is the default case
void menu(int menu_num) //only for non-address menus
{
  if (MENU_DEBUG) Serial.printf("menu %i\n", menu_num);
  menu_active = true;
  switch (menu_num) {
    case ADD_MENU:
      rolling_menu(menu_num);
      break;
    case FREQ_MENU:
      rolling_menu(menu_num);
      break;
    case EXIT_MENU:
      //exit menu and back to regular radio display
      break;
    case SET_MENU:
      #ifdef HAS_GPS
      draw_regular_menu(menu_num, MAX_ITEMS);
      #else
      draw_regular_menu(MAX_ITEMS - 3);
      #endif
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      if (tx_lock) {
        display.drawString(52, 20, "on");
      } else {
        display.drawString(52, 20, "off");
      }
      if (short_pause) {
        display.drawString(52, 35, "on");
      } else {
        display.drawString(52, 35, "off");
      }
      display.display();
      break;
    default: //all other menus, including main menu
      draw_regular_menu(menu_num, MAX_ITEMS);
  }
}

//as the user turns the rotary knob, a new item becomes the current item
//this highlights the current item by drawing a box around it
void highlight_item(int menu, int old_item, int new_item)
{
  //first erase the current item highlight
  display.setColor(BLACK);
  display.drawRect(pos[old_item][0] - DX, pos[old_item][1] - DY, 50, 15);  // x y is top left
  display.setColor(WHITE);
  display.drawRect(pos[new_item][0] - DX, pos[new_item][1] - DY, 50, 15);  // x y is top left
  display.display();
}

//this is the function that is called when the rotary button is pressed for a short click
// it activates the current menu item
// if it is a main menu item, it goes to the submenu
// if it is a radio menu item, it activates the choice on the radio
// for example, if the power level is changed in the menu, the new power level is set on the radio
// it does not write to nvram unless that menu item is picked in the settings menu
// so the changes are temporary until the settings are saved to nvram
bool activate(int cmenu, int citem)
{
  bool status = true; //it will be set to false if there is an error
  if (cmenu == MAIN_MENU) {  
    // activated in main menu, no radio actions, just go to correct menu
    bool circleValues = true;
    int secondary_menu_number = citem + 1;  //menu number is main menu item number + 1
    if (MENU_DEBUG) Serial.printf("activating menu  %i %s in main menu\n", secondary_menu_number, menu_label[secondary_menu_number]);
    switch (secondary_menu_number) {  //item 0 POWER_MENU menu 1, 1 MOD_MENU no 2,and 4 SET_MENU use regular menus in case default below
      //item 0 POWER_MENU, 1 MOD_MENU,and 4 SET_MENU use regular menus in case default below
      case ADD_MENU:  //ADD_MENU
        if (MENU_DEBUG) Serial.println("going to address menu");
        //show the current address
        cur_menu = ADD_MENU;
        cur_item[cur_menu] = act_item[cur_menu];  //set activated item as the default selected item
        //set up for address changes with the rotary knob
        //set boundaries and if values should cycle or not
        rotary.setBoundaries(1, ADDRESS_MAX, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        rotary.disableAcceleration(); //disable acceleration 
        rotary.reset(cur_item[cur_menu]); //sets the current value for the rotary encoder to the activated value
        rolling_menu(cur_menu);
        break;
      case FREQ_MENU: //FREQ_MENU
        if (MENU_DEBUG) Serial.println("going to frequency menu");
        //show the current address
        cur_menu = FREQ_MENU;
        cur_item[cur_menu] = act_item[cur_menu]; //set the activated item as the default selected item
        //set up for address changes with the rotary knob
        rotary.setBoundaries(0, FREQUENCY_INDEX_MAX, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        rotary.disableAcceleration(); //disable acceleration 
        rotary.reset(cur_item[cur_menu]); //sets the current value for the rotary encoder to the activated value
        rolling_menu(cur_menu);
        break;
      case EXIT_MENU: //Exit to radio
        if (MENU_DEBUG) Serial.println("Exiting menu");
        //exit menu and back to regular radio display
        menu_active = false;
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setColor(WHITE);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, "Exiting Menu - Back to Radio");
        display.display();
        rotary.setBoundaries(0, MAX_ITEMS - 1, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        rotary.disableAcceleration(); //disable acceleration 
        //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
        break;
      case SET_MENU: //SET_MENU
        if (MENU_DEBUG) Serial.println("going to settings menu");
        cur_menu = SET_MENU;
        cur_item[cur_menu] = act_item[cur_menu]; //set the activated item as the default selected item
        menu(cur_menu);  //draw the current menu
        #ifdef HAS_GPS
        //set up for address changes with the rotary knob
        rotary.setBoundaries(0, MENU_ARRAY_SIZE - 1, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        #else
        rotary.setBoundaries(0, MENU_ARRAY_SIZE - 4, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        #endif //HAS_GPS
        rotary.disableAcceleration(); //disable acceleration 
        //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
        break;
      default: //all other menus
        rotary.setBoundaries(0, MENU_ARRAY_SIZE - 1, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
        rotary.disableAcceleration(); //disable acceleration 
        //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
	      // go to submenu
        cur_menu = citem + 1;  //menu number is main menu item number + 1
	      menu(cur_menu); //go to the submenu
    } //switch
  } else { //secondary menu item, activate on radio
    cur_menu = cmenu;
    cur_item[cmenu] = citem;
	  act_item[cmenu] = citem;  //activate this on the radio
    //now save in preferences and activate on radio
    bool new_status;
    if (MENU_DEBUG) Serial.printf("in activate cur_menu %i %s cur_item %i %s\n", cur_menu, menu_label[cur_menu], cur_item[cur_menu], item_label[cur_menu][cur_item[cur_menu]]);
    switch (cur_menu) {
      case POWER_MENU:
        PARMS.parameters.power_index = citem;
        new_status = PARMS.set_power();
        status = status && new_status;
        break;
      case MOD_MENU:
        PARMS.parameters.modulation_index = citem;
        new_status = PARMS.set_modulation();
        status = status && new_status;
        break;
      case ADD_MENU:
        PARMS.parameters.address = citem;
        PARMS.set_address();
        break;
      case FREQ_MENU:
        PARMS.parameters.frequency_index = citem;
        new_status = PARMS.set_frequency();
        status = status && new_status;
        break;
      case SET_MENU:
        switch (citem) {
          case 0: //TX LOCK 
            tx_lock = !tx_lock;
            break;
          case 1: //short pause
            short_pause = !short_pause;
            break;
          case 2: //WRITE NV
            PARMS.update();
            break;
          default:
            PARMS.parameters.gps_state = citem - 3;
            break;
        } //switch item in settings menu
        break;
     } //switch secondary menus
  } //secondary menu item
    return status;
}


void on_button_short_click()
{
  //select and activate menu item
  if (MENU_DEBUG) Serial.printf("Short click cur_menu %i %s cur_item %i %s\n", cur_menu, menu_label[cur_menu], cur_item[cur_menu], item_label[cur_menu][cur_item[cur_menu]]);
  if (!activate(cur_menu, cur_item[cur_menu])) Serial.println("Failed to activate");
	if (!(cur_menu == MAIN_MENU && cur_item[cur_menu] + 1 == EXIT_MENU)) menu(cur_menu); //show the menu again unless exiting
}
void on_button_long_click()
{
  if (MENU_DEBUG) Serial.println("long click\n");
  cur_menu = 0;
  menu_active = true;
  act_item_init(); //pick up the activated values, needed for the main menu items
  menu(0);
}

void robot()
{
  // robot for menu demo
  int old = 0;
  for (int i = 0; i < 6; i++) 
  {
    highlight_item(0, old, i);
  	delay(1000);
   	old = i;
  }
  highlight_item(0,5,0);
}

// #ifndef ROTARY_H
// #define ROTARY_H 
// extern AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(RE_PIN_A, RE_PIN_B, RE_PIN_SW, RE_VCC_PIN, RE_STEPS);
// #endif


void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotary.isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
    return;
  }

  //button is up
  if (wasButtonDown) {
    //click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}
 
void draw_regular_menu(int menu_num, int max_items){
      display.clear();
      //menu header
      display.setFont(ArialMT_Plain_16);
      display.setColor(WHITE);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 0, menu_label[menu_num]);

      //now for the items
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      for (int i=0; i< MAX_ITEMS; i++) 
      {
	    if (menu_num != MAIN_MENU && i == act_item[menu_num]) {
          display.setFont(Dialog_bolditalic_10); //different font for the activated value in non main menu
	    } else {
          display.setFont(ArialMT_Plain_10);
	    }
	    display.drawString(pos[i][0], pos[i][1], item_label[menu_num][i]);
      };
  
      //higlight the current item 
      display.drawRect(pos[cur_item[menu_num]][0] - DX, pos[cur_item[menu_num]][1] - DY, 50, 15);
      display.display();
}
#endif //HAS_ENCODER