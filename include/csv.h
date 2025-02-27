#ifndef CSV_H
#define CSV_H

typedef struct CsvData {
    unsigned long timeStamp;
    char recvType;
    int from;
    int to;
    uint8_t headerId;
    float rssi;
    float snr;
    char gridLocator[11];
    double miles;
} CSVDATA, *CSVDATAPTR;

class CsvClass : public Print { //we derive from Print so that we can use printSplitter 
    public:
        CsvClass(Print& _printObject);
        virtual size_t write(uint8_t c); //this outputs as info 
        virtual size_t write(const char* str); //this outputs as info
        void broadcast(unsigned long timeStamp,uint8_t from , uint8_t headerId, char *gridLocator);
        void data(CSVDATAPTR data);
        void data(unsigned long timeStamp,char recvType, int from, int to, uint8_t headerID, float rssi, float snr, char* gridLocator = NULL);
        void info(const char *threeCharTag, char *data);
        void error(const char *threeCharTag, char *data);
        void fatalError(const char *threeCharTag, char *data);
        void debug(const char *threeCharTag, char *data);
    private:
        void output(char leadingChar,const char *tag, char *data);
        Print& printObject;
};
#if 0
class CsvPrintSplitter : public CsvClass {
  public:
    CsvPrintSplitter(CsvClass &_a, CsvClass &_b);
    size_t write(uint8_t c);
    size_t write(const char* str);
  private:
    CsvClass &a;
    CsvClass &b;
};
#endif

#endif //CSV_H