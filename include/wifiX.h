#ifndef WIFICLASS_H
#define WIFICLASS_H
#if USE_WIFI > 0
class WifiClass {
    public:
        void setup();
        bool init();
        void disconnect();
        bool changeAP();
};

extern WifiClass WIFI;
#endif //USE_WIFI > 0
#endif