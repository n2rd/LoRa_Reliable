#include "main.h"
#ifndef WM5500_H
#define WM5500_H

#include "Ethernet.h"

class wm5500Class {
    public:
        wm5500Class();
        void setup();
        EthernetClass& getEthernet();
    private:
        SPIClass spi;
        EthernetClass myEthernet;
};

extern wm5500Class WM5500;

#endif //WM5500_H