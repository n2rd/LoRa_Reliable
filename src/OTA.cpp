#include "main.h"

//////////////////////// OTA  stuff /////////////////////////

#if defined(USE_OTA) && (USE_OTA > 0)
#if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
#include <ESPAsyncWebServer.h>
#include "OTA.h"
#if defined(USE_ARDUINO_OTA) && (USE_ARDUINO_OTA == 1)
  #include <ArduinoOTA.h>
#endif
/////// Async Version ///////
bool otaActive = false;
unsigned long ota_progress_millis = 0;
static AsyncWebServer server(80);

void onOTAStart() {
  // Log when OTA has started
  log_i("OTA update started!");
  driver.setModeIdle();
  otaActive = true;
  p2pStop();
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    log_i("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    log_i("OTA update finished successfully!");
  } else {
    log_i("There was an error during OTA update!");
  }
  //driver.setMode(driver.RHModeRx);
  otaActive = false;
}

void ota_setup(void) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    char buffer[120];
    int sz = sprintf(buffer,"Welcome ! This is a %s device.\r\n\r\nThis radio is #%u\r\n\r\nFirmware Version built on: %s %s\r\n",PRODUCT_NAME,manager.thisAddress(),BUILD_DATE,BUILD_TIME);
    if (sz >= sizeof(buffer))
      log_e("size of buffer needed to be %d and is %d MEMORY HAS BEEN OVERWRITTEN !!!!",sz,sizeof(buffer));
    request->send(200, "text/plain",buffer);
  });

  // https://docs.elegantota.pro/getting-started/installation
  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  //ElegantOTA.setFWVersion("0.1.0"); //In Pro version only
  //ElegantOTA.setID("my_device_001"); //In Pro version only
  ElegantOTA.setAuth("Lora","Reliable");

  server.begin();
  log_i("HTTP server started");
  #if defined(USE_ARDUINO_OTA) && (USE_ARDUINO_OTA == 1)
    ArduinoOTA.begin();
  #endif
}

void ota_loop(void) {
  ElegantOTA.loop();
  #if defined(USE_ARDUINO_OTA) && (USE_ARDUINO_OTA == 1)
    ArduinoOTA.handle();
  #endif
}

#else
/////// Blocking Version ///////
unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void ota_setup(void) {
  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is Lora_Reliable. BLOCKING");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("HTTP server started");
}

void ota_loop(void) {
  server.handleClient();
  ElegantOTA.loop();
}
#endif
#endif //USE_OTA > 0
//////////////////////////////////////////////////////////////////// l