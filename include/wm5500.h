#include "main.h"
#ifndef WM5500_H
#define WM5500_H

class wm5500Class {
    public:
    wm5500Class();
    void setup();

    private:
    SPIClass spi;
};

#endif //WM5500_H