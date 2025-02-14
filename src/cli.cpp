/*

CLI syntax:
    Commands are case insensitive
    Embedded spaces ignored

Responses:
    OK = Command accepted
    NG = Command rejected with optional reason code

Required commands
    Callsign                Callsign is required for transmission in the ham bands. Any radio configured for ham 
                            band transmission will not transmit unless a callsign is configured.

CLI Command set

    Beacon                  \B <OFF|ON>                                     Default = on
    Callsign				\C <call>
    Frequency               \F <n>                                          Defailt = 902.5
                                    n= Frequency in MHz 3.g., 902.5, 
    GPS                     \G <OFF|ON>                                     Defailt = ON
                                    
    Help                    \H
    Transmission interval   \I <n>  n= number of seconds between            Default = 30
                                     transmission intervals (i.e., the 
                                     period)
    Location (fixed)        \L <n,m>                                        Default = 43.0351893,-76.1805654
                                    n= station latitude  (e.g.,  43.0103786)
                                    m= station longitude (e.g., -76.2850347)
	Modulation				\M <n>                                          Default = 7
                                    n=0 Short Turbo
                                    n=1 Short Fast
                                    n=2 Short Slow
                                    n=3 Medium Fast
                                    n=4 Medium Slow
                                    n=5 Long Fast
                                    n=6 Long Moderate
                                    n=7 Long Slow
                                    n=8 Very Long Slow
    Power                   \P <n>                                          Default = 6
                                    n=0  -9.0 dBm
                                    n=1  -5.0 dBm
                                    n=2  +0.0 dBm
                                    n=3  +6.0 dBm
                                    n=4 +12.0 dBm
                                    n=5 +18.0 dBm
                                    n=6 +22.0 dBm
    Quit (Telnet)           \Q
    Radio                   \R                                              Default = 1
    Serial USB Output       \S <OFF|ON>                                     Default = on
*/


#include "main.h"
#if 0
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#endif

void upcase(char* str) {

    int i;

    i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}


void removeBlanks(char* str) {
    int i, j = 0;

    for (i = 0; str[i] != '\0'; i++) {
        if (!isspace(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}


int cli_execute(const char* command_arg) {
    //temporary variables pending integration into ui.h
    char callsign[10];
    float frequency;
    bool gps_state;
    float lat_value, lon_value;
    int modulation_index;
    int power_index;
    int tx_interval;
    int radio_id;
    //

char command[50];
char cmd_code;
char* param_str[50];
char lat_str[10];
char lon_str[10];
int i, j;
int int_input;
float flt_input;
int status;
bool location_comma_found;
int lat_i, lon_i;

strcpy(command, command_arg);

if (command[0] == '\\') {
    upcase(command);                //upcase command

    cmd_code = command[1];          //snag command
    command[0] = ' ';               //replace leading \ and command with blanks
    command[1] = ' ';
    removeBlanks(command);          //remove blanks



    switch (cmd_code) {

        //      Beacon Off/On-------------------------------------------------------------

    case 'B':
        if (strcmp(command, "OFF") == 0) {
            gps_state = false;
            Serial.printf("OK, Beacon state = %d", gps_state);
        }
        else if (strcmp(command, "ON") == 0) {
            gps_state = true;
            Serial.printf("OK, Beacon state = %d", gps_state);
        }
        else
            Serial.printf("NG, Beacon state must be 'OFF' or 'ON'\n");

        break;

        //      Call Sign--------------------------------------------------------------
    case 'C':
        strcpy(callsign, command);
        Serial.printf("OK, Call sign = %s\n", callsign);
        break;

        //      Frequency--------------------------------------------------------------

    case 'F':
        flt_input = atof(command);
        if (flt_input >= 0 && flt_input <= 1000) {
            frequency = flt_input;
            Serial.printf("OK, Frequency = %6.3f\n", frequency);
        }
        else {
            Serial.printf("NG, Transmit interval must be between >=???? and <=????\n");
        }

        break;

        //      GPS Off/On-------------------------------------------------------------

    case 'G':
        if (strcmp(command, "OFF") == 0) {
            gps_state = false;
            Serial.printf("OK, GPS state = %d", gps_state);
        }
        else if (strcmp(command, "ON") == 0) {
            gps_state = true;
            Serial.printf("OK, GPS state = %d", gps_state);
        }
        else
            Serial.printf("NG, GPS state must be 'OFF' or 'ON'\n");

        break;

        //      Help-------------------------------------------------------------------
    case 'H':
        Serial.printf("Caallsign                   \C <callsign>\n");
        Serial.printf("Frequency                   \F <Frequency in MHz>\n");
        Serial.printf("GPS State                   \G <off>|<on>\n");
        Serial.printf("Help Text                   \H\n");
        Serial.printf("TX Interval (seconds)       \I <n>\n");
        Serial.printf("Position                    \L <latitude >,<longitude>\n");
        Serial.printf("Modulation index 0<=n<=8    \M <n>\n");
        Serial.printf("Power index 0<=n<=6         \P <n>\n");
        Serial.printf("Radio ID                    \R <n>\n");
        Serial.printf("Commands case insensitive and blanks ignored\n");

        telnet.printf("Caallsign                   \C <callsign>\n");
        telnet.printf("Frequency                   \F <Frequency in MHz>\n");
        telnet.printf("GPS State                   \G <off>|<on>\n");
        telnet.printf("Help Text                   \H\n");
        telnet.printf("TX Interval (seconds)       \I <n>\n");
        telnet.printf("Position                    \L <latitude >,<longitude>\n");
        telnet.printf("Modulation index 0<=n<=8    \M <n>\n");
        telnet.printf("Power index 0<=n<=6         \P <n>\n");
        telnet.printf("Radio ID                    \R <n>\n");
        telnet.printf("Commands case insensitive and blanks ignored\n");

        break;

        //      Interval (transmit)----------------------------------------------------

    case 'I':
        int_input = atoi(command);
        if (int_input >= 0 && int_input <= 8) {
            tx_interval = int_input;
            Serial.printf("OK, Transmit interval = %u\n", tx_interval);
        }
        else {
            Serial.printf("NG, Transmit interval must be >=???? and <=????\n");
        }
        break;

        //      Location---------------------------------------------------------------
    case 'L':

        location_comma_found = false;
        i = 0;
        lat_i = 0;
        lon_i = 0;
        lat_str[0] = '\0';
        lon_str[0] = '\0';
        while (command[i] != '\0') {
            if (command[i] != ',') {
                if (location_comma_found) {
                    if (lon_i < sizeof(lon_str)-1) {
                        lon_str[lon_i] = command[i];
                        lon_i++;
                    }
                    else {
                        Serial.printf("NG, Internal longitude buffer exceeded\n");
                        return 1;
                    }
                }
                else {
                    if (lat_i < sizeof(lat_str)-1) {
                        lat_str[lat_i] = command[i];
                        lat_i++;
                    }
                    else {
                        Serial.printf("NG, Internal latitude buffer exceeded\n");
                        return 1;
                    }
                }
            }
            else {
                    location_comma_found = true;
            }
            i++;
        }
        lat_str[lat_i] = '\0';
        lon_str[lon_i] = '\0';

        if (!location_comma_found) {
            Serial.printf("NG, Location requires comma separation between Lat & Lon\n");
            return 1;
        }
        lon_value = atof(lon_str);
        lat_value = atof(lat_str);

        if (abs(lon_value) > 180) {
            Serial.printf("NG, Longitude must be between -180 and 180\n");
            return 1;
        }
        if (abs(lat_value) > 90) {
            Serial.printf("NG, Longitude must be between -90 and 90\n");
            return 1;
        }
        Serial.printf("OK, Latitude = % f; Longitude = %f\n", lat_value, lon_value);
        break;

//      Modulation-------------------------------------------------------------
        case 'M':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 8) {
                modulation_index = int_input;
                Serial.printf("OK, Modulation_index = %u\n", modulation_index);
            }
            else {
                Serial.printf("NG, Modulation index must be >=0 and <=8\n");
            }
            break;

//      Power------------------------------------------------------------------
        case 'P':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 6) {
                power_index = int_input;
                Serial.printf("OK, Power_index = %u\n", power_index);
            }
            else {
                Serial.printf("NG, Power index must be >=0 and <=6\n");
            }
            break;

            //      Quit Telnet session----------------------------------------
        case 'Q':
            int_input = atoi(command);
            Serial.printf("OK, Quitting Telnet session\n");

            break;

            //      Radio ID---------------------------------------------------------------
        case 'R':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 10) {
                radio_id = int_input;
                Serial.printf("OK, Radio ID = %u\n", radio_id);
            }
            else {
                Serial.printf("NG, Radio ID must be >=0 and <=??????\n");
            }

            break;

            //      Serial USB Output Off/On-----------------------------------

        case 'S':
            if (strcmp(command, "OFF") == 0) {
                gps_state = false;
                Serial.printf("OK, Serial USB output state = %d", gps_state);
            }
            else if (strcmp(command, "ON") == 0) {
                gps_state = true;
                Serial.printf("OK, Serial USB output state = %d", gps_state);
            }
            else
                Serial.printf("NG, Serial USB output state must be 'OFF' or 'ON'\n");

            break;

            //      Invalid Command--------------------------------------------------------
        default:
            Serial.printf("NG, Unrecognized command %c [C, F, G, H, I, L, M, P, R]\n", cmd_code);
        }
    }
    else {
        Serial.printf("NG, Leading backslash missing\n");
        return 1;
    }

    return 0;
}


