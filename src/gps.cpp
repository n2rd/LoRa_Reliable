#include "main.h"
#if defined(HAS_GPS) && (HAS_GPS ==1)
#ifndef TESTGPS_TASK
#define TESTGPS_TASK 0
#endif //TESTGPS_TASK

// TIME esp32 internal RTC
#include <ESP32Time.h>  //includes time.h, uses the RTC built into ESP32
ESP32Time rtc(0);  // stay on UTC, neg or pos offset in seconds

#ifndef GPS_DEBUG  
#define GPS_DEBUG 0
#endif //GPS_DEBUG

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
    //yield();
  }
}
#endif //ARDUINO_ARCH_ESP32

void GPSClass::setup() {
  pinMode(GPS_ON_PIN, OUTPUT);  
  digitalWrite(GPS_ON_PIN, HIGH);//supply power to the GPS
  powerState = GPS_ON;
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  #if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 1
    xTaskCreatePinnedToCore(GPSTask,"GPSTask",10000,this,1,&GPSTaskHandle, xPortGetCoreID() == 1 ? 0 : 1);
  #endif //ARDUINO_ARCH_ESP32
}


void GPSClass::onoff(PowerState state) {
  //turn on or off the GPS
  // if the state is GPS_TX, the GPS will be turned on only when transmitting
  // the serial port does not care if the GPS is on or off
  if (state == powerState)
    return;
  if (state == GPS_ON) {
    digitalWrite(GPS_ON_PIN, HIGH);
  } else {
    digitalWrite(GPS_ON_PIN, LOW);
  }
  powerState = state;
}

GPSClass::PowerState GPSClass::onoffState()
{
  return powerState;
}

void GPSClass::loop()
{
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }
}
bool GPSClass::getLocation(double *lat, double *lng, double *alt, double *hdop) {
  bool gps_fix = false;
  unsigned long gps_start = millis();
  if (GPS_DEBUG) Serial.print("GPS:");
  
  #if !defined(ARDUINO_ARCH_ESP32) || TESTGPS_TASK == 2
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }
  #endif //!defined(ARDUINO_ARCH_ESP32) || TESTGPS_TASK == 0

  loop(); //Need this here otherwise it doesn't work even if called in main loop why?????

  gps_fix = gps.location.isUpdated() && gps.location.isValid();
  
  if (!gps_fix && (millis() - gps_start) > GPS_TIMEOUT * 1000) {
    display.println("GPS: No fix\n");
    log_e("GPS NO Fix");
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
#if 0
char* GPSClass::latLonToMaidenhead(double latitude, double longitude, int precision = 6) {
    if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
        return (char *)"Invalid coordinates";
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

    precision -= 4;

    for (int i = 1; i < precision /* / 2 */; ++i) {
        longitude *= 12;
        latitude *= 24;
        log_e("Grid6+ Lon %d, Lat %d", (int)longitude, (int)latitude);
        locator += (char)('a' + (int)(longitude));
        locator += (char)('a' + (int)(latitude));
        longitude = fmod(longitude, 1);
        latitude = fmod(latitude, 1);
    }
    log_e("locator.length =%d\r\n",locator.length());

    char *retLocator = (char *)malloc(locator.length()+1);

    return strcpy(retLocator, locator.c_str());
}
#endif
#if 0
#include "maidenhead.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#endif //0
char letterize(int x, bool upper) {
  if (upper)
    return (char) x + 65;
  else
    return (char) (x + (int)'a');
}
char* GPSClass::latLonToMaidenhead(double lat, double lon, int size = 6) {
    static char locator[11];
    double LON_F[]={20,2.0,0.083333,0.008333,0.0003472083333333333};
    double LAT_F[]={10,1.0,0.0416665,0.004166,0.0001735833333333333};
    int i;
    lon += 180;
    lat += 90;

    if (size <= 0 || size > 10) size = 6;

    for (i = 0; i < size/2; i++){
        if (i % 2 == 1) {
            locator[i*2] = (char) (lon/LON_F[i] + '0');
            locator[i*2+1] = (char) (lat/LAT_F[i] + '0');
        } else {
            locator[i*2] = letterize((int) (lon/LON_F[i]), i < 2);
            locator[i*2+1] = letterize((int) (lat/LAT_F[i]), i < 2);
        }
        lon = fmod(lon, LON_F[i]);
        lat = fmod(lat, LAT_F[i]);
    }
    locator[i*2]=0;
    return locator;
}

static char* complete_mh(char* locator) {
    static char locator2[11]; // = "LL55LL55LL";
    strcpy(locator2,"LL55LL55LL");
    log_e("locator2: %s",locator2);
    int len = strlen(locator);
    if (len >= 10) return locator;
    memcpy(locator2, locator, strlen(locator));
    return locator2;
}

static double mh2lon(char* locator) {
    double field, square, subsquare, extsquare, precsquare;
    int len = strlen(locator);
    if (len > 10) return 0;
    if (len < 10) locator = complete_mh(locator);
    field      = (locator[0] - 'A') * 20.0;
    square     = (locator[2] - '0') * 2.0;
    subsquare  = (locator[4] - 'A') / 12.0;
    extsquare  = (locator[6] - '0') / 120.0;
    precsquare = (locator[8] - 'A') / 2880.0;
    return field + square + subsquare + extsquare + precsquare - 180 - 2.67777; /* last is correction factor - bad math?? */
}

static double mh2lat(char* locator) {
    double field, square, subsquare, extsquare, precsquare;
    int len = strlen(locator);
    if (len > 10) return 0;
    if (len < 10) locator = complete_mh(locator);
    field      = (locator[1] - 'A') * 10.0;
    square     = (locator[3] - '0') * 1.0;
    subsquare  = (locator[5] - 'A') / 24.0;
    extsquare  = (locator[7] - '0') / 240.0;
    precsquare = (locator[9] - 'A') / 5760.0;
    return field + square + subsquare + extsquare + precsquare - 90.0 - 1.33888; /* last is correction factor - bad math?? */
}

void GPSClass::maidenheadGridToLatLon(char* grid,double *lat, double *lon) {
  *lat = mh2lat(complete_mh(grid));
  *lon = mh2lon(complete_mh(grid));
}

const char *GPSClass::getPowerStateName(GPSClass::PowerState state){
  return powerStateNames[(int)state];
}
#endif //defined(HAS_GPS) && (HAS_GPS ==1)