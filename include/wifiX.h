#ifndef WIFICLASS_H
#define WIFICLASS_H
#if USE_WIFI > 0
class WifiClass {
    public:
        void setup();
        bool init();
        void disconnect();
        bool changeAP();
    private:
    static void connectToWIFITask(void *pvParameter);
    static TaskHandle_t wifixCTWTaskHandle;
    void notifyConnected();
    void notifyDisconnected();
};

extern WifiClass WIFI;
#endif //USE_WIFI > 0
#endif