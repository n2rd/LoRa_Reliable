#include "main.h"

/*----------------------------------------------------*/
CsvClass::CsvClass(Print& _printObject) : printObject(_printObject) {}
/*----------------------------------------------------*/
uint count = 0;
char buffer[100];

size_t CsvClass::write(uint8_t c) { //this outputs as info
    buffer[count++] = c;
    buffer[count] = 0;
    if ((c == 0) || (c== '\n') || (c== '\r') ||(count > (sizeof(buffer)-2)))

    {
        info("INF",buffer);
        count = 0;
    }
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
        "D, %ld, %c, %d, %d, %2u, %.0f, %.0f\r\n",
        data->timeStamp,
        data->recvType,
        data->from,
        data->to,
        data->headerId,
        data->rssi,
        data->snr
    );
}
/*----------------------------------------------------*/
void CsvClass::data(unsigned long timeStamp,char recvType, int from, int to, uint8_t headerId, float rssi, float snr)
{
    CSVDATA datastruct;
    datastruct.timeStamp = timeStamp;
    datastruct.recvType = recvType;
    datastruct.from = from;
    datastruct.to = to;
    datastruct.headerId = headerId;
    datastruct.rssi = rssi;
    datastruct.snr = snr;
    data(&datastruct);
}
/*----------------------------------------------------*/
void CsvClass::output(char leadingChar,const char *tag, char *data)
{
    printObject.printf("%c,%s,%s\r\n",leadingChar,tag,data);

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
#if 0
class foo : public Print {
    public:
        foo(CsvPrintSplitter&& _a);
        size_t write(uint8_t ch);
        size_t write(const char* str);
    private:
        CsvPrintSplitter& a;
};
foo::foo(CsvPrintSplitter&& _a) : a(_a) {}
size_t foo::write(uint8_t ch) {
    size_t retval =  a.write(ch);
    return retval;
}
size_t foo::write(const char* str) {
    size_t retval = a.write(str);
    return retval;
}
/*----------------------------------------------------*/
CsvPrintSplitter::CsvPrintSplitter(CsvClass &_a, CsvClass &_b) : CsvClass(foo(this)), a(_a), b(_b) {}

size_t CsvPrintSplitter::write(uint8_t ch) {
    a.write(ch);
    size_t retval =  b.write(ch);
    return retval;
}
size_t CsvPrintSplitter::write(const char* str) {
    a.write(str);
    size_t retval = b.write(str);
    return retval;
}
#endif