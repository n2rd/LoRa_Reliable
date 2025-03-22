#ifndef OTA_H
#define OTA_H

#ifndef USE_WM5500_ETHERNET
extern bool otaActive;
void ota_loop(void);
void ota_setup(void);
#endif  //USE_WM5500_ETHERNET
#endif //OTA_H