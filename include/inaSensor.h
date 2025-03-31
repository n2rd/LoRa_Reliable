#ifndef inaSensor_h
#define inaSensor_h
#include <Adafruit_INA219.h>

class INASensor {
    public:
        void setup();
        void outputData(Print& printDev); 
        bool isPresent();
        float getBusVoltage_V();
        float getCurrent_mA(); 
    private:
        bool bIsPresent;
        Adafruit_INA219 ina219;
};

extern INASensor INA219;

//#endif //HAS_INA219
#endif
