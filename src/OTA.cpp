#ifndef USE_WM5500_ETHERNET
#include "main.h"
#include "OTA.h"


//////////////////////// OTA  stuff /////////////////////////
#if defined(USE_WIFI) && (USE_WIFI >0)
#if defined(ELEGANTOTA_USE_ASYNC_WEBSERVER) && ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
/////// Async Version ///////
bool otaActive = false;
unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  driver.setModeIdle();
  otaActive = true;
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
  driver.setMode(driver.RHModeRx);
  otaActive = false;
}

void ota_setup(void) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    char buffer[100];
    sprintf(buffer,"Hi! This is a Lora_reliable device.\r\n\r\nThis radio is #%u ASYNC",manager.thisAddress());
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
  Serial.println("HTTP server started");
}

void ota_loop(void) {
  ElegantOTA.loop();
}

#else
/////// Blocking Version ///////
bool otaActive = false;
unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  otaActive = true;
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
  otaActive = false;
}

void ota_setup(void) {
  Serial.begin(115200);
  #if defined(USE_WM5500_ETHERNET) && (USE_WM5500_ETHERNET == 1)
  #else //!defined(USE_WM5500_ETHERNET) || (USE_WM5500_ETHERNET != 1)
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  #endif //USE_WM5500_ETHERNET == 1
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
#endif
#endif //USE_WM5500_ETHERNET
//////////////////////////////////////////////////////////////////// l