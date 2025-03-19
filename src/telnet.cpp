#include "main.h"
#if USE_WIFI > 0
Telnet telnet;

const uint16_t  port = 23;

/* ------------------------------------------------- */
size_t Telnet::printf(const char* format, ...) {
  if (!client || !isConnected()) return 0;
  
  va_list arg;
  va_start(arg, format);
  char loc_buf[64];
  int len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
  va_end(arg);

  if (len < 0) return 0;

  if (len >= (int)sizeof(loc_buf)) {
    char* temp = (char*)malloc(len + 1);
    if (temp == nullptr) {
      return 0;
    }
    va_start(arg, format);
    vsnprintf(temp, len + 1, format, arg);
    va_end(arg);
    len = write((uint8_t*)temp, len);
    free(temp);
  } else {
    len = write((uint8_t*)loc_buf, len);
  }

  return len;
}
/* ------------------------------------------------- */
/* ------------------------------------------------- */

void Telnet::errorMsg(String error, bool restart)
{
    Serial.println(error);
    if (restart) {
    Serial.println("Rebooting now...");
    delay(2000);
    ESP.restart();
    delay(2000);
    }
}

/* ------------------------------------------------- */

// (optional) callback functions for telnet events
void Telnet::onTelnetConnect(String ip)
{
    csv_serial.printf("Telnet: %s connected\n", ip);
    telnet.printf("\nWelcome %s\n", telnet.getIP());
    telnet.printf("(Use ^] + q  to disconnect.)\r\n");
}

void Telnet::onTelnetDisconnect(String ip)
{
    csv_serial.printf("Telnet: %s disconnected\n", ip);
}

void Telnet::onTelnetReconnect(String ip)
{
    csv_serial.printf("Telnet: %s reconnected\n", ip);
}

void Telnet::onTelnetConnectionAttempt(String ip)
{
    csv_serial.printf("Telnet: %s tried to connect\n", ip);
}

void Telnet::onTelnetInput(String str)
{
    if (telnet.isLineModeSet()) {
        // checks" for a certain command
        int result=cli_execute(str.c_str());
        #if 0
        if (str == "ping") {
            telnet.println("> pong"); 
            Serial.println("- Telnet: pong");
        // disconnect the client
        } else if (str == "bye") {
            telnet.println("> disconnecting you...");
            telnet.disconnectClient();
        } else {
            telnet.println(str);
        }
        #endif
    }
    else {
        telnet.print(str);
        //Serial.print(str.length());
        Serial.print(str);
        //Serial.flush(true);
    }
}
/* ------------------------------------------------- */

void Telnet::setup()
{  
    // passing on functions for various telnet events
    telnet.onConnect(onTelnetConnect);
    telnet.onConnectionAttempt(onTelnetConnectionAttempt);
    telnet.onReconnect(onTelnetReconnect);
    telnet.onDisconnect(onTelnetDisconnect);
    telnet.onInputReceived(onTelnetInput);
    telnet.setLineMode(true);

    if (telnet.begin(port)) {
        csv_serial.info("TEL",(char*)"telnet running\n");
    } else {
        csv_serial.debug("TEL",(char*)"telnet error.\n");
        errorMsg("Will reboot...");
    }
}

/* ------------------------------------------------- */

void Telnet::loop()
{
    ESPTelnet::loop();

    // send serial input to telnet as output
    if (Serial.available()) {
        telnet.print(Serial.read());
    }
}
#endif //USE_WIFI > 0
//* ------------------------------------------------- */