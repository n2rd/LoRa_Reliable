#include "main.h"
#if 1
#include "Ethernet.h"

wm5500Class WM5500;

wm5500Class::wm5500Class()
{
    log_e("wm5500Class constructor core = %d",xPortGetCoreID());
    //setup();
}

void wm5500Class::setup()
{
    log_e("starting wm5500Class::setup() core = %d",xPortGetCoreID());
    //pinMode(WM5500_RST,OUTPUT);
    //digitalWrite(WM5500_RST,1);
    //delay(400);
    //digitalWrite(WM5500_RST,0);
    //delay(400);
    spi = new SPIClass(0);
    //spi = &SPI;
    if (spi != NULL) {
        log_e("spi!=NULL");
        spi->end();
        spi->begin(WM5500_SCLK,WM5500_MISO,WM5500_MOSI,WM5500_CS);
        EthernetClass myEthernet(spi);
        IPAddress myIP(192,168,9,240);
        uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04};
        myEthernet.begin(mac,myIP);
    }
    else
        log_e("spi is NULL");
    log_e("exiting wm5500Class::setup() core = %d",xPortGetCoreID());
}

EthernetClass& wm5500Class::getEthernet()
{
    return myEthernet;
}
#endif // 1