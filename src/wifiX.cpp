#include "main.h"

void WifiClass::setup() {}
bool WifiClass::init()
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("Lora_Reliable");
    WiFi.begin(PARMS.parameters.wifiSSID, PARMS.parameters.wifiKey);
    // Wait for connection
    int tryCount = 0;
    while ((WiFi.status() != WL_CONNECTED) && (tryCount < 8)) {
        delay(500);
        Serial.print(".");
        tryCount++;
    }
    if (tryCount == 8) {
        Serial.println(); //End dots
        display.println("Failed to connect to");
        display.println(PARMS.parameters.wifiSSID);
        Serial.println("Failed to connect to");
        Serial.println(PARMS.parameters.wifiSSID);
        return false;
    }
    // Not sure these are needed --- should investigate what they do exactly
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(PARMS.parameters.wifiSSID);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    display.print("Connected to ");
    display.println(PARMS.parameters.wifiSSID);
    display.print("IP: ");
    display.println(WiFi.localIP());
    return true;
}

void WifiClass::disconnect()
{
    WiFi.disconnect(false,false);
}
bool WifiClass::changeAP()
{
    disconnect();
    return init();
}


WifiClass WIFI;
