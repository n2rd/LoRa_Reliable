#include "main.h"

wm5500Class::wm5500Class()
{
    setup();
}

void wm5500Class::setup()
{
    spi.begin(WM5500_SCLK,WM5500_MISO,WM5500_MOSI,WM5500_CS);

    
}