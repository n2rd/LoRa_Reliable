#include "main.h"

/*----------------------------------------------------*/
CsvClass::CsvClass(Print& _printObject) : printObject(_printObject), outputEnabled(true)
    {}
/*----------------------------------------------------*/
uint count = 0;
char buffer[100];
/*----------------------------------------------------*/
/*
bool CsvClass::checkOutput()
{
    if (&printObject == &Serial) {
        //Got a Serial print object
        return serialOnOff;
    } 
    else if (&printObject == &telnet) {
        //Got a telnet print object
        return telnetOnOff;
    }
    return false;
}
*/    
/*----------------------------------------------------*/
#define CHECKOUTPUT if(!outputEnabled) return
/*----------------------------------------------------*/
void CsvClass::setOutputEnabled(bool state)
{
    outputEnabled = state;
}

/*----------------------------------------------------*/
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
void CsvClass::broadcast(unsigned long timeStamp,uint8_t from , uint8_t headerId, char *gridLocator)
{
    CHECKOUTPUT;
    char nul = 0;
    double myLat,myLon,bcastLat,bcastLon;
    double miles = -1;
    if (gridLocator) {
        GPS.maidenheadGridToLatLon(gridLocator,&bcastLat,&bcastLon);
        GPS.getLastLatLon(&myLat, &myLon);
        miles = GPS.distance(myLat, myLon, bcastLat, bcastLon);
    }
    printObject.printf(
        "B, %ld, O, %3d, 255, %3u,     ,    , %s, %4.2lf\r\n",
        timeStamp,
        from,
        headerId,
        gridLocator == NULL ? &nul : gridLocator,
        miles
        );  
}
/*----------------------------------------------------*/
void CsvClass::signalReport(unsigned long timeStamp,uint8_t from, uint8_t to , uint8_t headerId, char *gridLocator)
{
    CHECKOUTPUT;
    char nul = 0;
    double myLat,myLon,bcastLat,bcastLon;
    double miles = -1;
    if (gridLocator) {
        GPS.maidenheadGridToLatLon(gridLocator,&bcastLat,&bcastLon);
        GPS.getLastLatLon(&myLat, &myLon);
        miles = GPS.distance(myLat, myLon, bcastLat, bcastLon);
    }
    printObject.printf(
        "R, %ld, R, %3d, %3d, %3u,     ,    , %s, %4.2lf\r\n",
        timeStamp,
        from,
        to,
        headerId,
        gridLocator == NULL ? &nul : gridLocator,
        miles
        );  
}
/*----------------------------------------------------*/
void CsvClass::data(CSVDATAPTR data) 
{
    CHECKOUTPUT;
    printObject.printf(
        "D, %ld, %c, %3d, %3d, %3u, %4.0f, %3.0f, %s, %4.2lf\r\n",
        data->timeStamp,
        data->recvType,
        data->from,
        data->to,
        data->headerId,
        data->rssi,
        data->snr,
        data->gridLocator,
        data->miles
    );
}
/*----------------------------------------------------*/
void CsvClass::data(unsigned long timeStamp,char recvType, int from, int to, uint8_t headerId, float rssi, float snr, char* gridLocator)
{
    CHECKOUTPUT;
    CSVDATA datastruct;
    datastruct.timeStamp = timeStamp;
    datastruct.recvType = recvType;
    datastruct.from = from;
    datastruct.to = to;
    datastruct.headerId = headerId;
    datastruct.rssi = rssi;
    datastruct.snr = snr;
    if (gridLocator)
        strncpy(datastruct.gridLocator, (const char *)gridLocator, 11);
    else
        datastruct.gridLocator[0] = 0;

    double myLat,myLon, dataLat, dataLon;
    double miles = -1;
    if (gridLocator) {
        GPS.maidenheadGridToLatLon(gridLocator,&dataLat,&dataLon);
        GPS.getLastLatLon(&myLat, &myLon);
        datastruct.miles = GPS.distance(myLat, myLon, dataLat, dataLon);
    }
    else datastruct.miles = -1;

    data(&datastruct);
}
/*----------------------------------------------------*/
void CsvClass::output(char leadingChar,const char *tag, char *data)
{
    CHECKOUTPUT;
    printObject.printf("%c,%s,%s\r\n",leadingChar,tag,data);
}
/*----------------------------------------------------*/
void CsvClass::info(const char *tag, char *data)
{
    CHECKOUTPUT;
    output('I',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::error(const char *tag, char *data)
{
    CHECKOUTPUT;
    output('E',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::fatalError(const char *tag, char *data)
{
    CHECKOUTPUT;
    output('E',tag,data);
    #warning "HALT/REBOOT needs to be implemented"
    //HALT/REBOOT here after disconnecting telnet session and ???
}
/*----------------------------------------------------*/
void CsvClass::debug(const char *tag, char *data)
{
    CHECKOUTPUT;
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