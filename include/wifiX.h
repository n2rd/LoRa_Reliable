#ifndef WIFICLASS_H
#define WIFICLASS_H
#if USE_WIFI > 0
class WifiClass {
    public:
        void setup();
        bool init(bool bReinit = false);
        void killTask();
        void disconnect(bool bRadioOff = false,bool eraseAP = false);
        bool changeAP();
    private:
        static void connectToWIFITask(void *pvParameter);
        static TaskHandle_t wifixCTWTaskHandle;
        void notifyConnected();
        void notifyDisconnected();
        //----------------------//
        bool bIsReIniting;
        char hostnameBuffer[13];
};

extern WifiClass WIFI;
#endif //USE_WIFI > 0
#endif