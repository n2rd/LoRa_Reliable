#include "main.h"
#include "ntp.h"

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
#endif //defined(USE_WIFI) && (USE_WIFI > 0)

#define DEFAULT_CAD_TIMEOUT 1000  //mS default Carrier Activity Detect Timeout
#define TIMEOUT     200  //for sendtoWait
#define RETRIES     3     //for sendtoWait

// some state variables



//
#if USE_TELNET > 0
CsvClass csv_telnet(telnet);
#else //USE_TELNET == 0
CsvClass csv_telnet((Print&)dummyPrintSplitter);
#endif //USE_TELNET == 0
CsvClass csv_serial(Serial);
PrintSplitter csv_both(csv_serial,csv_telnet);
PrintSplitter ps_both(Serial, display);
#if USE_TELNET > 0
PrintSplitter ps_st(Serial,telnet);
PrintSplitter ps_all(Serial,telnet, display);
#else //USE_TELNET == 0
PrintSplitter ps_st(Serial);
PrintSplitter ps_all(Serial,display);
#endif //USE_TELNET == 0

RHDatagram manager(driver, 0);  
bool broadcastOnly = false;

#if HAS_GPS
double lastLat = 0;
double lastLon = 0;
#endif //HAS_GPS

void initializeNetwork() {
  #ifdef USE_WM5500_ETHERNET
    WM5500_Setup();
  #endif
  #if USE_WIFI > 0
    WIFI.init();
  #endif
  #if USE_OTA > 0
    ota_setup();
  #endif
  #if USE_TELNET > 0
    telnet.setup();
  #endif
  }


/***********************************************************/
/***********************************************************/
void setup() 
{
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available

  #ifdef ARDUINO_LILYGO_T3_V1_6_1
  pinMode(LED_BUILTIN, OUTPUT);
  SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_CS);
  #endif
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

  initializeNetwork();

  //start the radio
  if (!manager.init()) 
  {
    display.println("Radio failed to initialize");
    ps_both.println("HALTING");
    while (1);
  }

  //set the radio parameters
  PARMS.set_frequency();
  PARMS.set_power();
  PARMS.set_modulation();
  PARMS.set_address();
  if(PARMS.parameters.radioType == 1)
    broadcastOnly = true;

  //You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
#ifdef ARDUINO_LILYGO_T3_V1_6_1
  driver.setPayloadCRC(true);
  driver.setCADTimeout(DEFAULT_CAD_TIMEOUT);  //Carrier Activity Detect Timeout 
#else
  // CAD doesn't work on the heltec SX1262 driver yet return line 1134 of RH_SX126x.cpp needs commenting to make it work
  // also adding a _cad=false; line to line 259 of RH_SX126x.cpp
  //driver.setCADTimeout(DEFAULT_CAD_TIMEOUT/4);  //Carrier Activity Detect Timeout 
  //The below is a protected function but not called anywhere.  //TODO: Investigate datasheet to see if this is useful.
  //driver.setRxBoostMode(bool boost, bool retain)
#endif

  driver.setPromiscuous(PARMS.parameters.promiscuousEnabled);

  // Battery
  float vbat = heltec_vbat();
  ps_all.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));

  p2pSetup(broadcastOnly);

#ifdef DUMP_PARTITIONS
  dumpPartitions();
#endif //DUMP_PARTITIONS

#if HAS_GPS
  GPS.onoff((GPSClass::GPSPowerState)PARMS.parameters.gps_state);
  Serial.printf("GPS Power State: %s\r\n", GPS.getPowerStateName((GPSClass::GPSPowerState)PARMS.parameters.gps_state));
#endif //HAS_GPS

if (GPS.onoffState() == GPSClass::GPS_OFF) {
  InitNTP();
}
else {
  dumpLatLon();
}

#if defined(HAS_ENCODER) && (HAS_ENCODER == 1)
   rotary_setup();
#endif

  bmp280_setup();
  if (bmp280_isPresent()) {
    Serial.printf("Temperature: %f\r\n",myBMP280.readTempF());
    display.printf("Temperature: %f\r\n",myBMP280.readTempF());
  }
  csv_telnet.setOutputEnabled(PARMS.parameters.telnetCSVEnabled);
  csv_serial.setOutputEnabled(PARMS.parameters.serialCSVEnabled);
  //log_e("exiting setup()");
}
/***********************************************************/
/***********************************************************/
const int SIB_SIZE = 100;
char sib[SIB_SIZE+1];
int sibIndex = 0;
void serial_input_loop()
{
  bool sendToCli = false;
  do {
    while(Serial.available()) {
      if (sibIndex == 0)
        Serial.println();
      char ch = Serial.read();
      sib[sibIndex] = ch;
      if ((ch == '/') && (sibIndex == 0))
        Serial.print("Command:/");
      else
        Serial.print(ch);
      if ((ch == '\r') || (ch == '\n')) {
        sib[sibIndex] = 0;
        sendToCli = true;
        break;
      }
      else if ((ch == 8) || (ch == 127)) {
        if (sibIndex > 0) {
          sib[--sibIndex] = 0;
        }
      }
      if (sibIndex >= SIB_SIZE) {
        sib[sibIndex] = 0;
        break;
      }
      sibIndex++;
    }
    if (sendToCli) {
      //log_e("executing cli %d '%s'",sibIndex,sib);
      cli_execute(sib);
      sibIndex = 0;
      sendToCli = false;
      sib[0]=0;
    }
  } while(sib[0] == '/'); 
}
/***********************************************************/
unsigned long loopTimer = 0;
unsigned long lastLoopTimer = 0;
/***********************************************************/
void loop()
{
  static bool doneBroadcasting = false;
  //first check the buttons
  #if defined(USE_WIFI) && (USE_WIFI ==0)
  const bool otaActive = false;
  #endif
  if (!otaActive) {
    check_button();
    #if defined(USE_TELNET) && (USE_TELNET >0)
      telnet.loop();
    #endif
    if (broadcastOnly && !doneBroadcasting) {
      broadcastOnlyLoop();
      doneBroadcasting = true;
    }
    else
      p2pLoop();
    #if defined(HAS_ENCODER) && (HAS_ENCODER == 1)
      rotary_loop();
    #endif
    #if HAS_GPS
      GPS.loop();
    #endif
    serial_input_loop();
  }
  #if defined(USE_OTA) && (USE_OTA >0)
    ota_loop();
  #endif

// #if HAS_GPS
//   dumpLatLon();
// #endif //HAS_GPS
  loopTimer = millis();
  if ((loopTimer / 10000)  >= ((lastLoopTimer / 10000) + 6)) {
    debugMessage((char*)"main loop() is alive");
    //log_e("rtcIsSet: %d GPS.gps.time.isValid(): %d",GPS.getRtcIsSet(),GPS.gps.time.isValid());
    /*
    if (otaActive)
      debugMessage((char*)"otaActive is TRUE");
    else
      debugMessage((char*)"otaActive is FALSE");
    */
    lastLoopTimer = loopTimer;
  }
} //loop

/***********************************************************/
/***********************************************************/
void check_button() 
{
    //button presses
  static uint32_t single_button_time = 0.0;
  static uint32_t double_button_time = 0.0;
  #ifdef ARDUINO_LILYGO_T3_V1_6_1
    //We don't have a button at this time so just return
    return;
  #endif

  button.update();

  // single click will wake up sleeping unit

  //long press puts to sleep
  if (button.pressedFor(1000)) 
  { //go to sleep
    // Visually confirm it's off so user releases button
    display.displayOff();
    // Deep sleep (has wait for release so we don't wake up immediately)
    heltec_deep_sleep();
  }
}

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
#ifdef DUMP_PARTITIONS
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
#endif //defined(DUMP_PARTITIONS)

#if HAS_GPS
void dumpLatLon()
{
  double lat;
  double lon;
  if (GPS.getLocation(&lat,&lon)) {
    if ((lat != lastLat) || (lon != lastLon)) {
      display.printf("Lat: %f \r\nLon: %f\n",lat,lon);
      lastLat = lat;
      lastLon = lon;
    }
  }
}
#endif //HAS_GPS

/***********************************************************/
/***********************************************************/