#include "main.h"

/*----------------------------------------------------*/
CsvClass CSV;
/*----------------------------------------------------*/
CsvClass::CsvClass()
{

}
/*----------------------------------------------------*/
void CsvClass::data(CSVDATAPTR data) 
{
    //output D, data
    if (telnet.isConnected()) {
        telnet.printf(
            "D, %d, %d, %f, %f\n",
            data->timeStamp,
            data->from,
            data->rssi,
            data->snr
        );
    }
    if (Serial.availableForWrite()) {
        telnet.printf(
            "D, %d, %d, %f, %f\n",
            data->timeStamp,
            data->from,
            data->rssi,
            data->snr
        );
    }
}
/*----------------------------------------------------*/
void CsvClass::data(int timeStamp, int from, float rssi, float snr)
{
    CSVDATA datastruct;
    datastruct.timeStamp = timeStamp;
    datastruct.from = from;
    datastruct.rssi = rssi;
    datastruct.snr = snr;
    data(&datastruct);
}
/*----------------------------------------------------*/
void output(char leadingChar, char *tag, char *data)
{
    //output leadingChar,tag,data
    if (telnet.isConnected())
        telnet.printf("%c,%s,%s\n",leadingChar,tag,data);
    if (Serial.availableForWrite())
        Serial.printf("%c,%s,%s\n",leadingChar,tag,data);
}
/*----------------------------------------------------*/
void CsvClass::info(char *tag, char *data)
{
    output('I',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::error(char *tag, char *data)
{
    output('E',tag,data);
}
/*----------------------------------------------------*/
void CsvClass::fatalError(char *tag, char *data)
{
    output('E',tag,data);
    //HALT/REBOOT here after disconnecting telnet session and ???
}
/*----------------------------------------------------*/
void CsvClass::debug(char *tag, char *data)
{
    output('G',tag,data);
}
/*----------------------------------------------------*/