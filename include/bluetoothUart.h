#ifndef BLUETOOTHUART_H
#define BLUETOOTHUART_H
#include "main.h"
#include <BLESerial.h>

// FOR ETL: Uncomment the following lines
// #include <Embedded_Template_Library.h>
// #include <etl/queue.h>
// #include <etl/circular_buffer.h>

class BLEPrint : public Print
{
    public:
        BLEPrint();
        size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));
        void setup();
        void loop();
        void disconnect();
        void reConnect();
        //using Print:print;
    protected:
        size_t write(uint8_t ch) override;
        size_t write(const char* str);
        size_t write(uint8_t* bytes, unsigned int size);
    private:
    String device_name = "Lora_RKR-";
    // FOR ETL: Uncomment one of the following lines
    // BLESerial<etl::queue<uint8_t, 255, etl::memory_model::MEMORY_MODEL_SMALL>> serialBLE;
    // OR
    // BLESerial<etl::circular_buffer<uint8_t, 255>> serialBLE;
    BLESerial<> serialBLE;
};

extern BLEPrint bleTerminal;
#endif //BLUETOOTHUART_H