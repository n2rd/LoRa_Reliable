#include "main.h"

#ifdef ARDUINO_LILYGO_T3_V1_6_1
  #include "myLilyGoT3.h"
#else
  // Turns the 'PRG' button into the power button, long press is off 
  #define HELTEC_DEFAULT_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
  #include "myHeltec.h"
#endif

#if defined(USE_WIFI) && (USE_WIFI >0)
  #if defined(ESP32)
    #if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
      AsyncWebServer server(80);
    #else
      WebServer server(80);
    #endif
  #endif //defined(ESP32)
#endif //deefined(USE_WIFI) && (USE_WIFI > 0)

// 2 bytes packet payload, logging data as csv on serial port
/***  logging format ***
*  server
*     millis, from, counter, rssi, snr, send_report_back_status
* 
*  client
*   successful
*     millis, counter, rssi, snr, rssi_reported_by_server, snr_report 
*   failed
*     millis, counter, "failed"
*/

#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout
#define TIMEOUT     200  //for sendtoWait
#define RETRIES     3     //for sendtoWait

int modulation_index = PARMS.parameters.modulation_index;
int power_index = PARMS.parameters.power_index;

CsvClass csv_telnet(telnet);
CsvClass csv_serial(Serial);
PrintSplitter csv_both(csv_serial,csv_telnet);
PrintSplitter ps_both(Serial, display);
PrintSplitter ps_all(Serial,telnet, display);
RHReliableDatagram manager(driver, PARMS.parameters.address);

/***********************************************************/
/***********************************************************/
void DisplayUpperRight(int count) {
  char buf[10];
  memset(buf,32,sizeof(buf));
  sprintf(buf+2,"%5d", count);
  display.setColor(BLACK);
  display.fillRect((display.getWidth()) - 32, 0, 32, 14);
  display.setColor(WHITE);
  display.setColor(INVERSE);
  display.drawString((display.getWidth()) - 30, 0, buf);
  display.display();
  display.setColor(INVERSE);
  display.display();
}
/***********************************************************/
/***********************************************************/
void toggleLED()
{
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
}

/***********************************************************/
/***********************************************************/

#include "esp_partition.h"
void dumpPartitions() {
  esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY,ESP_PARTITION_SUBTYPE_ANY,NULL);
  esp_partition_iterator_t first_iterator = iterator;
  if (!iterator) {
    Serial.println("No Partitions");
    return;
  }
  Serial.println("                        Partion Table");
  Serial.println("Address              Label             Size    Type SubType");
  const esp_partition_t *part;
  do {
    part = esp_partition_get(iterator);
    if (part == NULL) {
      Serial.println("partition value is NULL");
      return;
    }
    Serial.printf("0x%06X   ", part->address);
    Serial.printf("%15s           ", part->label);
    Serial.printf("0x%06X  ", part->size);
    Serial.printf("0x%02X  ", part->type);
    Serial.printf("0x%02X\n", part->subtype);
    iterator = esp_partition_next(iterator);
  } while (iterator != NULL);
  Serial.println();
  esp_partition_iterator_release(iterator);
}
/***********************************************************/
/***********************************************************/
void setup() 
{
  #ifdef ARDUINO_LILYGO_T3_V1_6_1
  pinMode(LED_BUILTIN, OUTPUT);
  SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_CS);
  #endif
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  delay(5000);
  ota_setup();
  telnet.setup();

  //display init
  #ifndef ARDUINO_LILYGO_T3_V1_6_1
  heltec_display_power(true);
  heltec_ve(true); //added to turn on display
  #endif //!deefined(ARDUINO_LILYGO_T3_V1_6_1)
  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.displayOn();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, (display.getHeight() / 3)-16 /* font height */, "Lora_Reliable");
  display.drawString(display.getWidth() / 2, (display.getHeight() / 3)*2-16 /* font height */, VERSION);
  display.display();
  delay(3000);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.cls();

  //start the radio
  if (!manager.init()) 
  {
    display.println("Radio failed to initialize");
    ps_both.println("HALTING");
    while (1);
  }

  uint8_t power_index = PARMS.parameters.power_index;
  uint8_t modulation_index = PARMS.parameters.modulation_index;

  if (PARMS.parameters.address == 1) {
    ps_all.printf("Server %.3f MHz\n", PARMS.frequency_index_to_frequency(PARMS.parameters.frequency_index));
  } else {
    ps_all.printf("Client/P2P #%i at %.3f MHz\n", PARMS.parameters.address, PARMS.frequency_index_to_frequency(PARMS.parameters.frequency_index));
  }
  ps_all.printf("%s %.3f dBm\n", MY_CONFIG_NAME[modulation_index], power[power_index]);
  driver.setFrequency(PARMS.frequency_index_to_frequency(PARMS.parameters.frequency_index));
  setModemConfig(modulation_index); //SF Bandwith etc
  driver.setTxPower(power[power_index]);
  //#define DEBUG_INCOMING_PACKETS
  #if defined(DEBUG_INCOMING_PACKETS) && defined(ARDUINO_LILYGO_T3_V1_6_1)
  driver.setPromiscuous(true);
  #endif
  //You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
#ifdef ARDUINO_LILYGO_T3_V1_6_1
  driver.setPayloadCRC(true);
  driver.setCADTimeout(DEFAULT_CAD_TIMEOUT);  //Carrier Activity Detect Timeout 
#else
// This doesn't work on the heltec SX1262 driver yet
#endif

  // Battery
  float vbat = heltec_vbat();
  ps_all.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));

  p2pSetup();

  dumpPartitions();
}

/***********************************************************/
/***********************************************************/
void loop()
{
  //first check the buttons
  check_button();
  ota_loop();
  telnet.loop();
  p2pLoop();
} //loop

/***********************************************************/
/***********************************************************/
void check_button() 
{
    //button presses
  #warning "verify these are only initialized to zero once"
  static uint32_t single_button_time = 0.0;
  static uint32_t double_button_time = 0.0;
  #ifdef ARDUINO_LILYGO_T3_V1_6_1
    //We don't have a button at this time so just return
    return;
  #endif

  button.update();

  //single click to change power
  if (button.isSingleClick()) 
  {
    uint32_t button_time = millis() - single_button_time;
    single_button_time = millis();
    //if button time is < 1000 then pick next value
    //else show current value
    if (button_time > 2000 ) 
    {
      ps_all.printf("Current Power %.3fdBm\n", power[power_index]);
      ps_both.println("Single press button to change\n");
    } else {
      power_index = (power_index + 1) % POWER_INDEX_MAX;
      driver.setTxPower(power[power_index]);
      ps_all.printf("New Power %.3fdBm\n", power[power_index]);
    }
  }
 
  //double click inits the radio
  if (button.isDoubleClick()) // start the radio 
  {
    uint32_t button_time = millis() - double_button_time;
    double_button_time = millis();
    //if button time is < 1000 then pick next value
    //else show current value
    if (button_time > 2000 ) 
    {
      ps_all.printf("modulation_index= %d\n",modulation_index);
      ps_all.printf("Current Modulation %i %s\n", MY_CONFIG_NAME[modulation_index]);
      ps_both.println("Double press button to change\n");
    } else {
      modulation_index = (modulation_index + 1) % MODULATION_INDEX_MAX;
      setModemConfig(modulation_index);
      ps_all.printf("Current Modulation %i %s\n", MY_CONFIG_NAME[modulation_index]);
    }
  }

  //long press puts to sleep
  if (button.pressedFor(1000)) 
  { //go to sleep
    // Visually confirm it's off so user releases button
    display.displayOff();
    // Deep sleep (has wait for release so we don't wake up immediately)
    heltec_deep_sleep();
  }
}