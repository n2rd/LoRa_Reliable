#ifndef TELNETEXT_H
#define TELNETEXT_H

#include <ESPTelnet.h>
#include <Print.h>

class Telnet : public ESPTelnet, public Print {
  public:
    Telnet() : ESPTelnet(), Print() {}

    size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));

    template<typename T>
    void print(const T& data) {
      ESPTelnet::print(data);
    }

    void setup();
    void loop();
    void restart();

  //protected:
    size_t write(uint8_t ch);
    size_t write(const char* str);
    size_t write(uint8_t* bytes, unsigned int size);

  private:
    static void errorMsg(String error, bool restart = true);
    static void onTelnetConnect(String ip);
    static void onTelnetDisconnect(String ip);
    static void onTelnetReconnect(String ip);
    static void onTelnetConnectionAttempt(String ip);
    static void onTelnetInput(String str);
    size_t bufWrite(uint8_t*data, int len);
    void bufDump();
    const uint16_t  port = 23;
};

extern Telnet telnet;

#endif //TELNETEXT_H