#include "main.h"
#include "CircularBuffer.hpp"

#if USE_TELNET > 0
Telnet telnet;

/* ------------------------------------------------- */
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
    len = bufWrite((uint8_t*)temp, len);
    free(temp);
  } else {
    len = bufWrite((uint8_t*)loc_buf, len);
  }

  return len;
}
/* ------------------------------------------------- */
/* ------------------------------------------------- */
size_t Telnet::write(uint8_t ch)
{
    //return ESPTelnet::write(ch);
    return write((uint8_t*)&ch,1);
}
/* ------------------------------------------------- */
/* ------------------------------------------------- */
size_t Telnet::write(const char* str)
{
    //return ESPTelnet::write((const uint8_t*)str,strlen(str));
    return write((uint8_t*)str,strlen(str));
}
/* ------------------------------------------------- */
/* ------------------------------------------------- */
size_t Telnet::write(uint8_t* bytes, unsigned int size)
{
    //return ESPTelnet::write((const uint8_t*)bytes , size);
    return bufWrite(bytes,size);
}

/* ------------------------------------------------- */
/* ------------------------------------------------- */
DECLARE_MUTEX(telnetBufferMutex);
CircularBuffer<uint8_t,10000> telnetBuffer;
size_t Telnet::bufWrite(uint8_t*data, int len)
{
    MUTEX_LOCK(telnetBufferMutex);
    for (int i = 0; i < len; i++)
        telnetBuffer.push(data[i]);
    MUTEX_UNLOCK(telnetBufferMutex);
    return len;
}
/* ------------------------------------------------- */
void Telnet::bufDump()
{
    MUTEX_LOCK(telnetBufferMutex);
    decltype(telnetBuffer)::index_t size = telnetBuffer.size();
    uint8_t *tempBuf = static_cast<uint8_t*>(malloc(size));
    telnetBuffer.copyToArray(tempBuf);
    telnetBuffer.clear();
    ESPTelnet::write(tempBuf,size);
    free(tempBuf);
    MUTEX_UNLOCK(telnetBufferMutex);  
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
CircularBuffer<char,1000> telDbgBuffer;

void telnetDebugOutput(char c) {
    //telnet.print(c);
    //Not implemented yet ... gets called but we need to 
    //put the output into a buffer and then let another
    //thread pick it up and push it to the telnet stream
    //since we are in an event_task we can't call telnet.print()

    if (telnet.isConnected() && (strchr(PARMS.parameters.csvFilter,'D') != NULL)) {
        if (!telDbgBuffer.isFull())
            telDbgBuffer.push(c);
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

    if (telnet.begin(port,false)) {
        MUTEX_INIT(telnetBufferMutex);
        telDbgBuffer.clear();
        ets_install_putc2(telnetDebugOutput);
        csv_serial.info("TEL",(char*)"telnet running\n");
    } else {
        csv_serial.debug("TEL",(char*)"telnet error.\n");
        errorMsg("Will reboot...");
    }
}
/* ------------------------------------------------- */
void Telnet::restart()
{
    telnet.stop();
    if (telnet.begin(port,false)) {
        csv_serial.info("TEL",(char*)"restart() telnet running on new IP \r\n");
    } else {
        csv_serial.debug("TEL",(char*)"restart() telnet.begin error.\r\n");
        errorMsg("Will reboot...");
    }   
}
/* ------------------------------------------------- */
#if 0
static unsigned long lastLoopTimer = 0;
static unsigned long loopAverage = 0;
static unsigned int loopCounter = 0;
static int loopSamples = 30000;
#endif //0
void Telnet::loop()
{
    #if 0
    loopAverage += (micros()-lastLoopTimer); 
    lastLoopTimer = micros();
    if (++loopCounter == loopSamples) {
        loopCounter = 0;
        loopAverage /= loopSamples;
        log_i("Average Telnet loop servicing in micros over %d samples: %d", loopSamples, loopAverage);
        loopAverage = 0;
        loopSamples =  isConnected() ? 3000 : 30000;
    }
    #endif
    ESPTelnet::loop();
    MUTEX_LOCK(telnetBufferMutex);
        for (decltype(telDbgBuffer)::index_t i = 0; i < telDbgBuffer.size(); i++) {
            char ch = telDbgBuffer.shift();
            telnetBuffer.push(ch);
            //if (ch == '\n')
            //    break;
        }
    MUTEX_UNLOCK(telnetBufferMutex);
    if (isConnected())
        bufDump();
}
#endif //USE_TELNET > 0
//* ------------------------------------------------- */