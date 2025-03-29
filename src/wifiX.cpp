#include "main.h"
#if USE_WIFI > 0
void WifiClass::setup() {}

void WifiClass::connectToWIFITask(void *pvParameter)
{
    WifiClass *me = (WifiClass *)pvParameter;
    const TickType_t tickDelay = 5000;
    int tryCount = 0;
    while ((WiFi.status() != WL_CONNECTED) && (tryCount < 8)) {
        delay(500);
        Serial.print(".");
        display.print(".");
        tryCount++;
    }
    if (tryCount == 8) {
        display.println("Failed to connect to");
        display.println(PARMS.parameters.wifiSSID);
        Serial.println("Failed to connect to");
        Serial.println(PARMS.parameters.wifiSSID);
    }
    else
        Serial.println(); //End dots
    bool bWasntConnected = true;
    bool bWasConnected = false;
    wl_status_t wLastStatus;
    wl_status_t wStatus  = (wl_status_t)7;
    while (true) {
            /* Wifi.status()
            WL_IDLE_STATUS      = 0,
            WL_NO_SSID_AVAIL    = 1,
            WL_CONNECTED        = 3,
            WL_CONNECT_FAILED   = 4,
            WL_CONNECTION_LOST  = 5,
            WL_DISCONNECTED     = 6
            */
        wLastStatus = wStatus;
        wStatus = WiFi.status();
        if ((wStatus == WL_CONNECTED) && bWasntConnected) {
            //We reconnected
            bWasntConnected = false;
            bWasConnected = true;
            me->notifyConnected();
        }
        else {
            if ((wStatus == WL_CONNECTION_LOST) || (wStatus == WL_DISCONNECTED) && (bWasConnected)) {
                //we disconnected or never was connected ??
                bWasntConnected = true;
                bWasConnected = false;
                me->notifyDisconnected();
            }
            else {
                if (wLastStatus != wStatus) {
                    display.printf("Unprocessed Wifi Status: %d",wStatus);
                    Serial.printf("Unprocessed Wifi Status: %d",wStatus);
                }
            }
        }
        vTaskDelay(tickDelay);
    }
}

TaskHandle_t WifiClass::wifixCTWTaskHandle;

void WifiClass::notifyConnected()
{
    Serial.print("Connected to ");
    Serial.print(PARMS.parameters.wifiSSID);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    display.print("Connected to ");
    display.print(PARMS.parameters.wifiSSID);
    display.print("\r\nIP: ");
    display.println(WiFi.localIP());
}

void WifiClass::notifyDisconnected()
{
    Serial.print("Disconnected from ");
    Serial.println(PARMS.parameters.wifiSSID);
    display.print("Disconnected from ");
    display.println(PARMS.parameters.wifiSSID);
}
bool WifiClass::init()
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("Lora_Reliable");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.begin(PARMS.parameters.wifiSSID, PARMS.parameters.wifiKey);
    xTaskCreatePinnedToCore(connectToWIFITask,"wifixCTWTask",10000,this,1,&wifixCTWTaskHandle, xPortGetCoreID());
    delay(4500); // Wait for connection or failure in Task
    return (WiFi.status() == WL_CONNECTED) ? true : false;
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
#endif //USE_WIFI > 0
