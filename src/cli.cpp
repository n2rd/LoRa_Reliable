/*

========================Definition snipits from Raj's code

PROGMEM static const RH_SX126x::ModemConfig MY_MODEM_CONFIG_TABLE[MODULATION_INDEX_MAX] =
{
    //  packetType, p1, p2, p3, p4, p5, p6, p7, p8
     // 0 Short Turbo
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_128, RH_SX126x_LORA_BW_500_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 1 Short Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_128, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 2 Short Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_256, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 3 Medium Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_512, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 4 Medium Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_1024, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 5 Long Fast
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_250_0, RH_SX126x_LORA_CR_4_5, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 6 Long Moderate
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_2048, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 7 Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_125_0, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
    // 8 Very Long Slow
    { RH_SX126x::PacketTypeLoRa, RH_SX126x_LORA_SF_4096, RH_SX126x_LORA_BW_62_5, RH_SX126x_LORA_CR_4_8, RH_SX126x_LORA_LOW_DATA_RATE_OPTIMIZE_OFF, 0, 0, 0, 0},
};

enum SF {
  SF_64 = 6,
  SF_128 = 7,
  SF_256 = 8,
  SF_512 = 9,
  SF_1024 = 10,
  SF_2048 = 11,
  SF_4096 = 12
};


=======================Code snippits from Raj's code
#define POWER_INDEX_MAX 7
float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};
int power_index = DEFAULT_POWER_INDEX;

  driver.setTxPower(power[power_index]);

        power_index = (power_index + 1) % POWER_INDEX_MAX;
      driver.setTxPower(power[power_index]);

      modulation_index = (modulation_index + 1) % MODULATION_INDEX_MAX;
      setModemConfig(modulation_index);
 


    driver.setFrequency(DEFAULT_FREQUENCY);

  driver.setSignalBandwidth(250000);
  driver.setSpreadingFactor(11);
  driver.setCodingRate4(5);


  ============================== From Raj's email of 2/15 @ 0950
  //preferences do not exist, initialize
     preferences.putUInt("address", 0);
     preferences.putInt("powerIndex", 0);  //lowest power, -9dBm for safety
     preferences.putInt("modIndex", 2);    //long-fast
     preferences.putInt("freqIndex", 5);   //915.0 Mhz
     preferences.putInt("gpsIndex", 0);    //off
     preferences.putString("call", "N2RD");
     preferences.putInt("interval", 30);   //time in seconds between transmissions
     preferences.putFloat("lat", 43.1);    //if no fix or no gps, may be set by cli
     preferences.putFloat("lng", -77.5);   //if no fix or no gps, may be set by cli
     preferences.putInt("radioType", 2);   //0 server, 1 client, 2 peer to peer
================================================


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

    Radio Address           /A                                              Default = 0
    Beacon                  /B <OFF|ON>                                     Default = on
    Callsign				/C <call>
    Frequency index         /F <n>                                          Default = 905.2
                                    n= Frequency in MHz 3.g., 905.2, 
    GPS                     /G <OFF|ON>                                     Default = ON
                                    
    Help                    /H
    Transmission interval   /I <n>  n= number of seconds between            Default = 30
                                     transmission intervals (i.e., the 
                                     period)
    Location (fixed)        /L <n,m>                                        Default = 43.0351893,-76.1805654
                                    n= station latitude  (e.g.,  43.0103786)
                                    m= station longitude (e.g., -76.2850347)
	Modulation index		/M <n>                                          Default = 7
                                    n=0 Short Turbo
                                    n=1 Short Fast
                                    n=2 Short Slow
                                    n=3 Medium Fast
                                    n=4 Medium Slow
                                    n=5 Long Fast
                                    n=6 Long Moderate
                                    n=7 Long Slow
                                    n=8 Very Long Slow
    Power index             /P <n>                                          Default = 6
                                    n=0  -9.0 dBm
                                    n=1  -5.0 dBm
                                    n=2  +0.0 dBm
                                    n=3  +6.0 dBm
                                    n=4 +12.0 dBm
                                    n=5 +18.0 dBm
                                    n=6 +22.0 dBm
    Quit (Telnet)           /Q
    Serial USB Output       /S <OFF|ON>                                     Default = on
    Radio Type              /T <n>                                          Default = 2
                                    n=0 Server (client/server operation)
                                    n=1 Client (client/server operation)
                                    n=2 Peer (peer to peer operation)
*/

#include "main.h"


// These need to move elsewhere=======================================
#warning "These need to move elsewhere"
#define POWER_INDEX_MAX 7
extern float power[POWER_INDEX_MAX];
#define MODULATION_INDEX_MAX 9
extern const char* MY_CONFIG_NAME[MODULATION_INDEX_MAX];
extern void setModemConfig(uint8_t i);                             //need to fix
//====================================================================



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
    float frequency; //depricated
    int frequency_index;
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

if (command[0] == '/') {
    upcase(command);                //upcase command

    cmd_code = command[1];          //snag command
    command[0] = ' ';               //replace leading \ and command with blanks
    command[1] = ' ';
    removeBlanks(command);          //remove blanks



    switch (cmd_code) {

            //      Radio Address----------------------------------------------
            case 'A':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 10) {
                radio_id = int_input;
                Serial.printf("OK:Radio Address = %u\n", radio_id);
                telnet.printf("OK:Radio Address = %u\r\n", radio_id);
            }
            else {
                Serial.printf("NG:Radio Address must be >=0 and <=??????\n");
                telnet.printf("NG:Radio Address must be >=0 and <=??????\r\n");
            }

            break;

        //      Beacon Off/On-------------------------------------------------------------

    case 'B':
        if (strcmp(command, "OFF") == 0) {
            gps_state = false;
            Serial.printf("OK:Beacon state = %d\n", gps_state);
            telnet.printf("OK:Beacon state = %d\r\n", gps_state);
        }
        else if (strcmp(command, "ON") == 0) {
            gps_state = true;
            Serial.printf("OK:Beacon state = %d\n", gps_state);
            telnet.printf("OK:Beacon state = %d\r\n", gps_state);
        }
        else {
            Serial.printf("NG:Beacon state must be 'OFF' or 'ON'\n");
            telnet.printf("NG:Beacon state must be 'OFF' or 'ON'\r\n");
        }

        break;

        //      Call Sign--------------------------------------------------------------
    case 'C':
        strcpy(callsign, command); 
        Serial.printf("OK:Call sign = %s\n", callsign);
        telnet.printf("OK:Call sign = %s\r\n", callsign);
       
        break;

        //      Frequency--------------------------------------------------------------

    case 'F':
        int_input = atoi(command);
        if (int_input >= 0 && int_input <= 1000) {
            frequency = int_input;
            Serial.printf("OK:Frequency = %s\n", frequency);
            telnet.printf("OK:Frequency = %s\r\n", frequency);
        }
        else {
            Serial.printf("NG:Frequency index must be between >=0 and <=????\n");
            telnet.printf("NG:Frequnecy index must be between >=0 and <=????\r\n");
        }

        break;

        //      GPS Off/On-------------------------------------------------------------

    case 'G':
        if (strcmp(command, "OFF") == 0) {
            gps_state = false;
            Serial.printf("OK:GPS state = %d\n", gps_state);
            telnet.printf("OK:GPS state = %d\r\n", gps_state);
         }
        else if (strcmp(command, "ON") == 0) {
            gps_state = true;
            Serial.printf("OK:GPS state = %d\n", gps_state);
            telnet.printf("OK:GPS state = %d\r\n", gps_state);
        }
        else {
            Serial.printf("NG:GPS state must be 'OFF' or 'ON'\n");
            telnet.printf("NG:GPS state must be 'OFF' or 'ON'\r\n");
        }

        break;

        //      Help-------------------------------------------------------------------
    case 'H':
        Serial.printf("Radio Address               /A <n>\n");
        Serial.printf("Beacon                      /B <off>|<on>\n");
        Serial.printf("Caallsign                   /C <callsign>\n");
        Serial.printf("Frequency                   /F <Frequency in MHz>\n");
        Serial.printf("GPS State                   /G <off>|<on>\n");
        Serial.printf("Help Text                   /H\n");
        Serial.printf("TX Interval (seconds)       /I <n>\n");
        Serial.printf("Position                    /L <latitude >,<longitude>\n");
        Serial.printf("Modulation index 0<=n<=8    /M <n>\n");
        Serial.printf("Power index 0<=n<=6         /P <n>\n");
        Serial.printf("USB Serial Output           /S <off>|<on>\n");
        Serial.printf("Radio Type                  /T <n>\n");
        Serial.printf("Commands case insensitive and blanks ignored\n");

        telnet.printf("Radio Address               /A <n>\r\n");
        telnet.printf("Beacon                      /B <off>|<on>\r\n");
        telnet.printf("Caallsign                   /C <callsign>\r\n");
        telnet.printf("Frequency                   /F <Frequency in MHz>\r\n");
        telnet.printf("GPS State                   /G <off>|<on>\r\n");
        telnet.printf("Help Text                   /H\r\n");
        telnet.printf("TX Interval (seconds)       /I <n>\r\n");
        telnet.printf("Position                    /L <latitude >,<longitude>\r\n");
        telnet.printf("Modulation index 0<=n<=8    /M <n>\r\n");
        telnet.printf("Power index 0<=n<=6         /P <n>\r\n");
        telnet.printf("USB Serial output           /S <off>|<on>\r\n");
        telnet.printf("Radio Type                  /T <n>\r\n");
        telnet.printf("Commands case insensitive and blanks ignored\r\n");

        break;

        //      Interval (transmit)----------------------------------------------------

    case 'I':
        int_input = atoi(command);
        if (int_input >= 0 && int_input <= 8) {
            tx_interval = int_input;
            Serial.printf("OK:Transmit interval = %u\n", tx_interval);
            telnet.printf("OK:Transmit interval = %u\r\n", tx_interval);
        }
        else {
            Serial.printf("NG:Transmit interval must be >=???? and <=????\n");
            telnet.printf("NG:Transmit interval must be >=???? and <=????\r\n");
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
                        Serial.printf("NG:Internal longitude buffer exceeded\n");
                        telnet.printf("NG:Internal longitude buffer exceeded\r\n");
                        return 1;
                    }
                }
                else {
                    if (lat_i < sizeof(lat_str)-1) {
                        lat_str[lat_i] = command[i];
                        lat_i++;
                    }
                    else {
                        Serial.printf("NG:Internal latitude buffer exceeded\n");
                        telnet.printf("NG:Internal latitude buffer exceeded\r\n");
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
            Serial.printf("NG:Location requires comma separation between Lat & Lon\n");
            telnet.printf("NG:Location requires comma separation between Lat & Lon\r\n");
            return 1;
        }
        lon_value = atof(lon_str);
        lat_value = atof(lat_str);

        if (abs(lon_value) > 180) {
            Serial.printf("NG:Longitude must be between -180 and 180\n");
            telnet.printf("NG:Longitude must be between -180 and 180\r\n");
            return 1;
        }
        if (abs(lat_value) > 90) {
            Serial.printf("NG:Longitude must be between -90 and 90\n");
            telnet.printf("NG:Longitude must be between -90 and 90\r\n");
            return 1;
        }
        Serial.printf("OK:Latitude = % f; Longitude = %f\n", lat_value, lon_value);
        telnet.printf("OK:Latitude = % f; Longitude = %f\r\n", lat_value, lon_value);

        break;

//      Modulation-------------------------------------------------------------
        case 'M':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 8) {
                modulation_index = int_input;
                modulation_index = (modulation_index + 1) % MODULATION_INDEX_MAX;
                setModemConfig(modulation_index);                             //need to fix
                Serial.printf("OK:Modulation_index = %u\n", modulation_index);
                telnet.printf("OK:Modulation_index = %u\r\n", modulation_index);
                          }
            else {
                Serial.printf("NG:Modulation index must be >=0 and <=8\n");
                telnet.printf("NG:Modulation index must be >=0 and <=8\r\n");
            }
            break;

//      Power------------------------------------------------------------------
        case 'P':
            int_input = atoi(command);
            if (int_input >= 0 && int_input <= 6) {
                power_index = int_input;
                driver.setTxPower(power[power_index]);
                Serial.printf("OK:Power_index = %u\n", power_index);
                telnet.printf("OK:Power_index = %u\r\n", power_index);
            }
            else {
                Serial.printf("NG:Power index must be >=0 and <=6\n");
                telnet.printf("NG:Power index must be >=0 and <=6\r\n");
            }
            break;

            //      Quit Telnet session----------------------------------------
        case 'Q':
            int_input = atoi(command);
            Serial.printf("OK:Quitting Telnet session\n");
            telnet.printf("OK:Quitting Telnet session\r\n");

            break;

            //      Serial USB Output Off/On-----------------------------------

        case 'S':
            if (strcmp(command, "OFF") == 0) {
                gps_state = false;
                Serial.printf("OK:Serial USB output state = %d\n", gps_state);
                telnet.printf("OK:Serial USB output state = %d\r\n", gps_state);
            }
            else if (strcmp(command, "ON") == 0) {
                gps_state = true;
                Serial.printf("OK:Serial USB output state = %d\n", gps_state);
                telnet.printf("OK:Serial USB output state = %d\r\n", gps_state);
            }
            else
                Serial.printf("NG:Serial USB output state must be 'OFF' or 'ON'\n");
                telnet.printf("NG:Serial USB output state must be 'OFF' or 'ON'\r\n");

            break;

        //      Radio Type-----------------------------------------------------
        case 'T':
        int_input = atoi(command);
        if (int_input >= 0 && int_input <= 2) {
            modulation_index = int_input;
            Serial.printf("OK:Radio Type = %u\n", modulation_index);
            telnet.printf("OK:Radio Type = %u\r\n", modulation_index);
        }
        else {
            Serial.printf("NG:Radio Type must be >=0 and <=8\n");
            telnet.printf("NG:Radio Type index must be >=0 and <=8\r\n");
        }
        break;

        //      Invalid Command--------------------------------------------------------
        default:
            Serial.printf("NG:Unrecognized command %c [C, F, G, H, I, L, M, P, R]\n", cmd_code);
            telnet.printf("NG:Unrecognized command %c [C, F, G, H, I, L, M, P, R]\r\n", cmd_code);
        }
    }
    else {
        Serial.printf("NG:Leading backslash missing\n");
        telnet.printf("NG:Leading backslash missing\r\n");
        return 1;
    }

    return 0;
}


