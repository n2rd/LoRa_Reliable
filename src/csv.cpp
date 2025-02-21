#include "main.h"

/*----------------------------------------------------*/
CsvClass::CsvClass(Print& _printObject) : printObject(_printObject) {}
/*----------------------------------------------------*/
size_t CsvClass::write(uint8_t c) { //this outputs as info
    char s[2];
    s[0] = c;
    s[1] = 0;
    info("INF",s);
    return 1;
}
/*----------------------------------------------------*/
size_t CsvClass::write(const char* str) { //this outputs as info 
    info("INF",(char *)str);
    return strlen(str);
}
/*----------------------------------------------------*/
void CsvClass::data(CSVDATAPTR data) 
{
    printObject.printf(
        "D, %ld, %d, %f, %f\n",
        data->timeStamp,
        data->from,
        data->rssi,
        data->snr
    );
}
/*----------------------------------------------------*/
void CsvClass::data(unsigned long timeStamp, int from, float rssi, float snr)
{
    CSVDATA datastruct;
    datastruct.timeStamp = timeStamp;
    datastruct.from = from;
    datastruct.rssi = rssi;
    datastruct.snr = snr;
    data(&datastruct);
}
/*----------------------------------------------------*/
void CsvClass::output(char leadingChar,const char *tag, char *data)
{
    printObject.printf("%c,%s,%s\n",leadingChar,tag,data);

}
/*----------------------------------------------------*/
void CsvClass::info(const char *tag, char *data)
{
    output('I',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::error(const char *tag, char *data)
{
    output('E',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::fatalError(const char *tag, char *data)
{
    output('E',tag,data);
    #warning "HALT/REBOOT needs to be implemented"
    //HALT/REBOOT here after disconnecting telnet session and ???
}
/*----------------------------------------------------*/
void CsvClass::debug(const char *tag, char *data)
{
    output('G',tag,data);
}
/*----------------------------------------------------*/