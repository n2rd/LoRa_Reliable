#include "main.h"

ESPTelnet telnet;

const uint16_t  port = 23;

/* ------------------------------------------------- */

void errorMsg(String error, bool restart = true)
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
void onTelnetConnect(String ip)
{
    Serial.print("- Telnet: ");
    Serial.print(ip);
    Serial.println(" connected");

    telnet.println("\nWelcome " + telnet.getIP());
    telnet.println("(Use ^] + q  to disconnect.)");
}

void onTelnetDisconnect(String ip)
{
    #warning "comment out or remove debuggging info"
    Serial.print("- Telnet: ");
    Serial.print(ip);
    Serial.println(" disconnected");
}

void onTelnetReconnect(String ip)
{
    Serial.print("- Telnet: ");
    Serial.print(ip);
    Serial.println(" reconnected");
}

void onTelnetConnectionAttempt(String ip)
{
    Serial.print("- Telnet: ");
    Serial.print(ip);
    Serial.println(" tried to connected");
}

void onTelnetInput(String str)
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

void telnet_setup()
{  
    // passing on functions for various telnet events
    telnet.onConnect(onTelnetConnect);
    telnet.onConnectionAttempt(onTelnetConnectionAttempt);
    telnet.onReconnect(onTelnetReconnect);
    telnet.onDisconnect(onTelnetDisconnect);
    telnet.onInputReceived(onTelnetInput);
    telnet.setLineMode(true);

    Serial.print("- Telnet: ");
    if (telnet.begin(port)) {
        Serial.println("telnet running");
    } else {
        Serial.println("telnet error.");
        errorMsg("Will reboot...");
    }
}

/* ------------------------------------------------- */

void telnet_loop()
{
    telnet.loop();

    // send serial input to telnet as output
    if (Serial.available()) {
        telnet.print(Serial.read());
    }
}

//* ------------------------------------------------- */