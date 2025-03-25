#ifndef OTA_H
#define OTA_H
#if defined(USE_OTA) && (USE_OTA > 0)
extern bool otaActive;
void ota_loop(void);
void ota_setup(void);
#endif
#endif //OTA_H