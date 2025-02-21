#ifndef CSV_H
#define CSV_H

typedef struct CsvData {
    unsigned long timeStamp;
    char recvType;
    int from;
    int to;
    float rssi;
    float snr;
} CSVDATA, *CSVDATAPTR;

class CsvClass : public Print { //we derive from Print so that we can use printSplitter 
    public:
        CsvClass(Print& _printObject);
        size_t write(uint8_t c); //this outputs as info 
        size_t write(const char* str); //this outputs as info 
        void data(CSVDATAPTR data);
        void data(unsigned long timeStamp,char recvType, int from, int to, float rssi, float snr);
        void info(const char *threeCharTag, char *data);
        void error(const char *threeCharTag, char *data);
        void fatalError(const char *threeCharTag, char *data);
        void debug(const char *threeCharTag, char *data);
    private:
        void output(char leadingChar,const char *tag, char *data);
        Print& printObject;
};

#endif //CSV_H