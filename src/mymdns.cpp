#include "main.h"
#include "mymdns.h"
#include <ESPmDNS.h>

void mdns_start()
{
    #if ((USE_WM5500_ETHERNET > 0) || (USE_WIFI > 0))
        //Do setup for mDNS advertising --- PRODUCT_NAME-<radio-address>.local
        char buffer[16];
        int sz = sprintf(buffer,"%s-%d",PRODUCT_NAME,PARMS.parameters.address);
        if (sz >= sizeof(buffer))
            log_e("BUFFER OVERRUN");
        if (!MDNS.begin(buffer)) {
            log_e("Error setting up MDNS responder!");
        }
        else {
            MDNS.addService("http", "tcp", 80);
            MDNS.addService("telnet", "tcp", 23);
        }
        log_i("mDNS responder started");
    #endif  
}

void mdns_stop()
{
    #if ((USE_WM5500_ETHERNET > 0) || (USE_WIFI > 0))
        MDNS.end();
    #endif    
}
