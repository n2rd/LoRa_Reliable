#ifndef CSV_H
#define CSV_H

typedef struct CsvData {
    int timeStamp;
    int from;
    float rssi;
    float snr;
} CSVDATA, *CSVDATAPTR;

class CsvClass {
    public:
        CsvClass();
        void data(CSVDATAPTR data);
        void data(int timeStamp, int from, float rssi, float snr);
        void info(char *threeCharTag, char *data);
        void error(char *threeCharTag, char *data);
        void fatalError(char *threeCharTag, char *data);
        void debug(char *threeCharTag, char *data);
};

extern CsvClass CSV;

#endif //CSV_H