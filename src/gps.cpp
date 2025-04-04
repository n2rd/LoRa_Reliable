#include "main.h"

#ifndef TESTGPS_TASK
#define TESTGPS_TASK 1
#endif //TESTGPS_TASK

#ifndef DEFAULT_GPS_BAUDRATE
#define DEFAULT_GPS_BAUDRATE 9600
#endif //DEFAULT_GPS_BAUDRATE

// TIME esp32 internal RTC

ESP32Time GPSClass::rtc(0);  // stay on UTC, neg or pos offset in seconds

#ifndef GPS_DEBUG  
#define GPS_DEBUG 0
#endif //GPS_DEBUG
#define TIME_DIFF_TO_UPDATE_ON 800
#if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 0
static HardwareSerial GPSSerial(2);    //use Hardware UART1 for GPS
#endif

GPSClass GPS;
//-----------------------------------------------------------------------------
//
//Constructor 
//
//-----------------------------------------------------------------------------
GPSClass::GPSClass()
{
  rtcIsSet = false;
  lastLat = 0;
  lastLon = 0;
  setup();
}
//-----------------------------------------------------------------------------
//
// GPSClass class FUNCTIONS
//

#if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 1
TaskHandle_t GPSTaskHandle;
unsigned long timeCheckValue = 0;
const unsigned long timeCheckValueCheckValue = 10000;
volatile bool hsErrorOccurred = false;
HardwareSerial *gpsSerialPtr;
char rxBuffer[2000];
bool firstReceive = true;
//-----------------------------------------------------------------------------
void hsErrorCb(hardwareSerial_error_t hsError) 
{
  if ((hsError == UART_FRAME_ERROR) || (hsError == UART_PARITY_ERROR) || (hsError == UART_BREAK_ERROR)) {
    log_d("caught hsErrorCb: %d", hsError);
    hsErrorOccurred = true;
    //switch to the next baud rate;
    uint32_t newBaudRate = (gpsSerialPtr->baudRate() == 9600) ? 115200 : 9600;
    gpsSerialPtr->updateBaudRate(newBaudRate);
    gpsSerialPtr->flush(false);
    log_d("Changing GPS BaudRate to %d", newBaudRate);
  }
}
//-----------------------------------------------------------------------------
void onReceive()
{
  int count;
  memset(rxBuffer,0, sizeof(rxBuffer));
  count = gpsSerialPtr->readBytes(rxBuffer,sizeof(rxBuffer));
  if(firstReceive) {
    rxBuffer[sizeof(rxBuffer)-1] = 0;
    char* subStr = strstr(rxBuffer,"$GP");
    if (subStr == NULL)
      subStr = strstr(rxBuffer,"$GL");
    if (subStr == NULL)
      subStr = strstr(rxBuffer,"$GN");
    if (!subStr) {
      //Change baud rate here ??
      log_d("RxBuffer: %s",rxBuffer);
    }
    firstReceive = false;
  }
  else {
    for (int i = 0; i < count; i++)
      GPS.gps.encode(rxBuffer[i]);
  }
}
//-----------------------------------------------------------------------------
void GPSClass::GPSTask(void *pvParameter)
{
  GPSClass* me = (GPSClass *)pvParameter;
  gpsSerialPtr = new HardwareSerial(2);
  if (gpsSerialPtr != NULL) {
    gpsSerialPtr->setRxBufferSize(2000);
    gpsSerialPtr->begin(DEFAULT_GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    gpsSerialPtr->onReceiveError(hsErrorCb);
    gpsSerialPtr->onReceive(onReceive);
    bool bForceUpdate = false;
    while (true) {
      if ((!me->rtcIsSet || bForceUpdate) && ((me->powerState == GPS_ON) || (me->powerState == GPS_TX))) {
        if (me->gps.time.isUpdated() && me->gps.time.isValid()
          && me->gps.date.isUpdated() && me->gps.date.isValid()) {
          rtc.setTime(
            me->gps.time.second() + (me->gps.time.age() /1000),
            me->gps.time.minute(),
            me->gps.time.hour(),
            me->gps.date.day(),
            me->gps.date.month(),
            me->gps.date.year(),
            0//(me->gps.time.centisecond() * (1000 *10)) + (me->gps.time.age() * 1000)
          );
          log_d("time set age: %d timestamp: %d",me->gps.time.age(),rtc.getLocalEpoch());
          me->rtcIsSet = true;
          bForceUpdate = false;
        }
      }
      else if (me->powerState == GPS_ON) { //rtc is set and GPS state is GPS_ON
        // Check the time every timeCheckCounterCheckValue times we come here. mainly happens when updating firmware.
        if (millis() >= timeCheckValue) {
          timeCheckValue = millis() + timeCheckValueCheckValue;
          uint8_t gpsSeconds = me->gps.time.second() + (me->gps.time.age() / 1000);
          //unsigned long rtcMillis = rtc.getMillis();
          int rtcSeconds = rtc.getSecond();
          me->timeDiff = (rtcSeconds *1000 + rtc.getMillis()) - (gpsSeconds*1000 + me->gps.time.centisecond() * 10);
          if (abs(me->timeDiff) > TIME_DIFF_TO_UPDATE_ON) {
            log_d("time to be updated. TimeDiff > %d: %d  Sats: %d hdop: %lf",TIME_DIFF_TO_UPDATE_ON,me->timeDiff,me->gps.satellites.value(),me->gps.hdop.hdop());
            log_d("GPS UTC Time: %4d-%02d-%02d  %02d:%02d:%02d",
              me->gps.date.year(),me->gps.date.month(),me->gps.date.day(),
              me->gps.time.hour(),me->gps.time.minute(),me->gps.time.second());
            //Time has drifted or improved via GPS so reset time.
            bForceUpdate = true;
          }
        }
      }
      if ((me->powerState == GPS_ON) || (me->powerState == GPS_TX))
        vTaskDelay((me->rtcIsSet ? 15000 : 20) / portTICK_PERIOD_MS);
      else
        vTaskDelay(1000*60*60 / portTICK_PERIOD_MS);
    } //while(true)
  }
}
#endif //ARDUINO_ARCH_ESP32
//-----------------------------------------------------------------------------
void GPSClass::setup() {
  pinMode(GPS_ON_PIN, OUTPUT);  
  digitalWrite(GPS_ON_PIN, HIGH);//supply power to the GPS
  powerState = GPS_ON;
  #if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 1
    xTaskCreatePinnedToCore(GPSTask,"GPSTask",10000,this,1,&GPSTaskHandle, xPortGetCoreID() == 1 ? 0 : 1);
  #else
    GPSSerial.begin(DEFAULT_GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  #endif //ARDUINO_ARCH_ESP32
  timeCheckValue = millis() + timeCheckValueCheckValue;
  baudSwitchReason = -1;
  baudTestBufferLen = -1;
}

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
GPSClass::PowerState GPSClass::onoffState()
{
  return powerState;
}
//-----------------------------------------------------------------------------
void GPSClass::loop()
{
#if defined(ARDUINO_ARCH_ESP32) && TESTGPS_TASK == 0
  while (GPSSerial.available() > 0) {
    char ch = GPSSerial.read();
    //Serial.print(ch);
    gps.encode(ch);
    if (gps.time.isUpdated()) {
      if (!rtcIsSet) {
        if (gps.time.isUpdated() && gps.time.isValid()
          && gps.date.isUpdated() && gps.date.isValid()) {
          rtc.setTime(
            gps.time.second(),
            gps.time.minute(),
            gps.time.hour(),
            gps.date.day(),
            gps.date.month(),
            gps.date.year(),
            0
          );
          rtcIsSet = true;
        }
      }
    }
  }
#endif
}
//-----------------------------------------------------------------------------
unsigned long GPSClass::getTimeStamp()
{
  if (rtcIsSet)
    return rtc.getLocalEpoch();
  else
    return millis() / 1000;
}

//-----------------------------------------------------------------------------
bool GPSClass::getLocation(double *lat, double *lng, double *alt, double *hdop) {
  bool gps_fix = false;
  unsigned long gps_start = millis();
  
  loop();

  gps_fix = gps.location.isUpdated() && gps.location.isValid();
  
  if (!gps_fix && (millis() - gps_start) > GPS_TIMEOUT * 1000) {
    display.println("GPS: No fix\n");
    log_d("GPS NO Fix");
    gps_start = millis();
  } 

  if (gps_fix) {
    gps_start = millis();

    lastLat = gps.location.lat();
    lastLon = gps.location.lng();

    if (lat)
      *lat = lastLat;
    if (lng)
      *lng = lastLon;
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
static char letterize(int x, bool upper) {
  if (upper)
    return (char) x + 65;
  else
    return (char) (x + (int)'a');
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
static char* complete_mh(char* locator) {
    static char locator2[11]; // = "LL55LL55LL";
    strcpy(locator2,"LL55LL55LL");
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
//-----------------------------------------------------------------------------
void GPSClass::maidenheadGridToLatLon(char* grid,double *lat, double *lon) {
  *lat = mh2lat(complete_mh(grid));
  *lon = mh2lon(complete_mh(grid));
}
//-----------------------------------------------------------------------------
const char *GPSClass::getPowerStateName(GPSClass::PowerState state){
  return powerStateNames[(int)state];
}
//-----------------------------------------------------------------------------
static double degreesToRadians(double d)
{
  return d * (PI / 180.0);
}
//-----------------------------------------------------------------------------
/// @brief returns distance in miles between 2 pairs of lat/lon data
/// @param lat1 
/// @param lon1 
/// @param lat2 
/// @param lon2 
/// @return distance between lat1/lon1 to lat2/lon2
//-----------------------------------------------------------------------------
double GPSClass::distance(double lat1, double lon1, double lat2, double lon2)
{
  const double earthRadius = 6371.0; //in kilometers
  double deltaRLat, deltaRLon;
  deltaRLat = degreesToRadians(lat2 - lat1);
  deltaRLon = degreesToRadians(lon2 - lon1);

  // Apply the Haversine formula
  double a = sin(deltaRLat / 2) * sin(deltaRLat / 2) + 
    cos(degreesToRadians(lat1)) * cos(degreesToRadians(lat2)) * 
    sin(deltaRLon / 2) * sin(deltaRLon / 2);
    
  double c = 2 * atan2(sqrt(a), sqrt(1 - a)); //calulate the central angle 
  double distanceInKilometers = earthRadius *c;
  double miles = distanceInKilometers / 1.6; //convert to miles
  return miles;
}
//-----------------------------------------------------------------------------
/// @brief caculates bearing between two lat/lon pairs
/// @param lat1 
/// @param lon1 
/// @param lat2 
/// @param lon2 
/// @return bearing from lat1/lon1 to lat2/lon2
//-----------------------------------------------------------------------------
double GPSClass::bearing(double lat1, double lon1, double lat2, double lon2)
 {
  // Convert latitude and longitude from degrees to radians
  lat1 = degreesToRadians(lat1);
  lon1 = degreesToRadians(lon1);
  lat2 = degreesToRadians(lat2);
  lon2 = degreesToRadians(lon2);

  // Calculate the difference in longitudes
  double delta_lon = lon2 - lon1;

  // Calculate y and x components for the bearing formula
  double y = sin(delta_lon) * cos(lat2);
  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(delta_lon);

  // Calculate the bearing in radians
  double bearing_rad = atan2(y, x);

  // Convert bearing from radians to degrees
  double bearing_deg = bearing_rad * 180.0 / PI;

  // Normalize bearing to be within the range [0, 360)
  bearing_deg = fmod(bearing_deg + 360.0, 360.0);

  return bearing_deg;
}
//-----------------------------------------------------------------------------
bool GPSClass::getLastLatLon(double *lat, double *lon)
{
  bool retVal = true;
  if ((lastLat == 0) && (lastLon == 0)) {
    //Lat/lon not set by GPS ... use fixed value
    *lat = PARMS.parameters.lat_value;
    *lon = PARMS.parameters.lon_value;
    retVal = false;
  }
  else {
    *lat = lastLat;
    *lon = lastLon;
  }
  return retVal;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------