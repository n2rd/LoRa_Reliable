#include "main.h"
#if USE_WIFI > 0
void WifiClass::setup() { bIsReIniting = false; }

void WifiClass::connectToWIFITask(void *pvParameter)
{
    WifiClass *me = (WifiClass *)pvParameter;
    sprintf(me->hostnameBuffer,"Lora_RKR-%d",PARMS.parameters.address);
    if (me->bIsReIniting) {
        WiFi.reconnect();
        me->bIsReIniting = false;
    }
    else {
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(me->hostnameBuffer);
        WiFi.setAutoReconnect(true);
        WiFi.persistent(true);
        WiFi.begin(PARMS.parameters.wifiSSID, PARMS.parameters.wifiKey);
    }
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
            WL_DISCONNECTED     = 6,

            ------------------------
            Disconnect reasons
            WL_ASSOC_LEAVE      = 8
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
            if ((wStatus == WL_CONNECTION_LOST) || (wStatus == WL_DISCONNECTED)) {
                if (bWasConnected) {
                    //we disconnected or never was connected ??
                    bWasntConnected = true;
                    bWasConnected = false;
                    me->notifyDisconnected();
                    //WiFi.disconnect(false,false); This caused the reconnection to not happen
                }
            }
            else if (wStatus == WL_IDLE_STATUS){
                if (WiFi.reconnect()) {
                    bWasntConnected = false;
                    bWasConnected = true;
                    me->notifyConnected();
                }
            }
            else {
                if (wLastStatus != wStatus) {
                    uint val = (bWasConnected ? 0x02 : 0x00) | (bWasntConnected ? 0x01 : 0x00);
                    display.printf("Unproc Wifi Status:%02X val: %02X\r\n",wStatus, val);
                    Serial.printf("Unprocessed Wifi Status: %02X val: %02X\r\n",wStatus, val);
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
bool WifiClass::init(bool bReinit)
{
    xTaskCreatePinnedToCore(connectToWIFITask,"wifixCTWTask",10000,this,1,&wifixCTWTaskHandle, xPortGetCoreID());
    delay(4500); // Wait for connection or failure in Task
    return (WiFi.status() == WL_CONNECTED) ? true : false;
}

void WifiClass::killTask()
{
    vTaskDelete(wifixCTWTaskHandle);
}

void WifiClass::disconnect(bool bRadioOff, bool bEraseAP)
{
    WiFi.disconnect(bRadioOff,bEraseAP);
}
bool WifiClass::changeAP()
{
    disconnect();
    return init();
}

WifiClass WIFI;
#endif //USE_WIFI > 0
