#ifndef WIFICLASS_H
#define WIFICLASS_H

class WifiClass {
    public:
        void setup();
        bool init();
        void disconnect();
        bool changeAP();
};

extern WifiClass WIFI;

#endif