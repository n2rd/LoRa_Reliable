#if defined(HAS_GPS) && (HAS_GPS ==1)
#include "Arduino.h"
#include "gps.h"
// TIME esp32 internal RTC
#include <ESP32Time.h>  //includes time.h, uses the RTC built into ESP32
ESP32Time rtc(0);  // stay on UTC, neg or pos offset in seconds
  
#define GPS_DEBUG 0
HardwareSerial GPSSerial(2);    //use Hardware UART1 for GPS
TinyGPSPlus gps;


//
// FUNCTIONS
//

void gps_setup() {
  pinMode(GPS_ON_PIN, OUTPUT);  
  digitalWrite(GPS_ON_PIN, HIGH);//supply power to the GPS 
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void gps_toggle(int state) {
  //turn on or off the GPS
  // if the state is GPS_TX, the GPS will be turned on only when transmitting
  // the serial port does not care if the GPS is on or off
  if (state == GPS_ON) {
    digitalWrite(GPS_ON_PIN, HIGH);
  } else {
    digitalWrite(GPS_ON_PIN, LOW);
  }
}

bool get_gps_location(double &lat, double &lng, double &alt, double &hdop) {
  bool gps_fix = false;
  unsigned long gps_start = millis();
  if (GPS_DEBUG) Serial.print("GPS:");
  
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }
  
  gps_fix = gps.location.isUpdated() && gps.location.isValid();
  
  if (!gps_fix && (millis() - gps_start) > GPS_TIMEOUT * 1000) {
    Serial.println("No fix\n");
    gps_start = millis();
  } 

  if (gps_fix) {
    gps_start = millis();
    lat = gps.location.lat();
    lng = gps.location.lng();
    alt = gps.altitude.meters();
    hdop = gps.hdop.hdop();
    rtc.setTime(gps.time.second(), gps.time.minute(), gps.time.hour(), 
                gps.date.day(), gps.date.month(), gps.date.year());
    if (GPS_DEBUG) {
      Serial.printf("\nGPS Data Follows\n");
      Serial.printf("LAT= %.6f\n", lat);
      Serial.printf("LONG= %.6f\n", lng);
      Serial.printf("ALT= %.1f\n", alt);
      Serial.printf("HDOP= %.2f\n", hdop);
      Serial.println(rtc.getTimeDate());
      Serial.println(latLonToMaidenhead(lat, lng, 6));
    }
  } 
  return gps_fix;
}

String latLonToMaidenhead(double latitude, double longitude, int precision = 6) {
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

uint16_t encode_grid4(String locator) {
  return ((locator[0] - 'A') * 18 + (locator[1] - 'A') )* 100 + (locator[2] - '0') * 10 + (locator[3] - '0') ;
}

void decode_grid4(uint16_t grid4, char *grid) {
  grid[0] = (grid4 / 180) + 'A';
  grid[1] = (grid4 % 180) / 10 + 'A';
  grid[2] = (grid4 % 100) / 10 + '0';
  grid[3] = (grid4 % 10) + '0';
  grid[4] = '\0';
}

#endif //defined(HAS_GPS) && (HAS_GPS ==1)