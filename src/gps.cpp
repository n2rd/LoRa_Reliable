#include "main.h"
#if defined(HAS_GPS) && (HAS_GPS ==1)
#ifndef TESTGPS_TASK
#define TESTGPS_TASK 0
#endif //TESTGPS_TASK

// TIME esp32 internal RTC
#include <ESP32Time.h>  //includes time.h, uses the RTC built into ESP32
ESP32Time rtc(0);  // stay on UTC, neg or pos offset in seconds
  
#define GPS_DEBUG 0
static HardwareSerial GPSSerial(2);    //use Hardware UART1 for GPS

GPSClass GPS;

//
//Constructor 
//
GPSClass::GPSClass()
{
  setup();
}
//
// GPSClass class FUNCTIONS
//

#if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 1
TaskHandle_t GPSTaskHandle;

void GPSClass::GPSTask(void *pvParameter)
{
  while (true) {
    GPSClass* me = (GPSClass *)pvParameter;
    while (GPSSerial.available() > 0) {
      me->gps.encode(GPSSerial.read());
    }
  }
}
#endif //ARDUINO_ARCH_ESP32

void GPSClass::setup() {
  pinMode(GPS_ON_PIN, OUTPUT);  
  digitalWrite(GPS_ON_PIN, HIGH);//supply power to the GPS 
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  #if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 1
    xTaskCreatePinnedToCore(GPSTask,"GPSTask",10000,this,1,&GPSTaskHandle, xPortGetCoreID() == 1 ? 0 : 1);
  #endif //ARDUINO_ARCH_ESP32
}


void GPSClass::onoff(PowerState state) {
  //turn on or off the GPS
  // if the state is GPS_TX, the GPS will be turned on only when transmitting
  // the serial port does not care if the GPS is on or off
  if (state == GPS_ON) {
    digitalWrite(GPS_ON_PIN, HIGH);
  } else {
    digitalWrite(GPS_ON_PIN, LOW);
  }
}

bool GPSClass::getLocation(double *lat, double *lng, double *alt, double *hdop) {
  bool gps_fix = false;
  unsigned long gps_start = millis();
  if (GPS_DEBUG) Serial.print("GPS:");
  
  #if !defined(ARDUINO_ARCH_ESP32) || TESTGPS_TASK == 0
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }
  #endif //!defined(ARDUINO_ARCH_ESP32) || TESTGPS_TASK == 0
  
  gps_fix = gps.location.isUpdated() && gps.location.isValid();
  
  if (!gps_fix && (millis() - gps_start) > GPS_TIMEOUT * 1000) {
    display.println("GPS: No fix\n");
    gps_start = millis();
  } 

  if (gps_fix) {
    gps_start = millis();

    if (lat)
      *lat = gps.location.lat();
    if (lng)
      *lng = gps.location.lng();
    if (alt)
      *alt = gps.altitude.meters();
    if (hdop)
      *hdop = gps.hdop.hdop();

    rtc.setTime(gps.time.second(), gps.time.minute(), gps.time.hour(), 
                gps.date.day(), gps.date.month(), gps.date.year());
    if (GPS_DEBUG) {
      Serial.printf("\nGPS Data Follows\n");
      if (lat)
        Serial.printf("LAT= %.6f\n", *lat);
      if (lng)
        Serial.printf("LONG= %.6f\n", *lng);
      if (alt)
        Serial.printf("ALT= %.1f\n", *alt);
      if (hdop)
        Serial.printf("HDOP= %.2f\n", *hdop);
      Serial.println(rtc.getTimeDate());
      if (lat && lng)
        Serial.println(latLonToMaidenhead(*lat, *lng, 6));
    }
  } 
  return gps_fix;
}

String GPSClass::latLonToMaidenhead(double latitude, double longitude, int precision = 6) {
    if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
        return "Invalid coordinates";
    }

    longitude += 180.0;
    latitude += 90.0;

    String locator = "";

    locator += (char)('A' + (int)(longitude / 20));
    locator += (char)('A' + (int)(latitude / 10));

    longitude = fmod(longitude, 20);
    latitude = fmod(latitude, 10);

    locator += (char)('0' + (int)(longitude / 2));
    locator += (char)('0' + (int)(latitude));

    longitude = fmod(longitude, 2);
    latitude = fmod(latitude, 1);
    
    for (int i = 1; i < precision / 2; ++i) {
        longitude *= 24;
        latitude *= 24;
        locator += (char)('a' + (int)(longitude));
        locator += (char)('a' + (int)(latitude));
        longitude = fmod(longitude, 1);
        latitude = fmod(latitude, 1);
    }

    return locator;
}

const char *GPSClass::getPowerStateName(GPSClass::PowerState state){
  return powerStateNames[(int)state];
}
#endif //defined(HAS_GPS) && (HAS_GPS ==1)