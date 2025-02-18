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
    Display                 /D
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
    Write NVRAM             /W
    Maidenhead Grid Square  /???????????????                                
*/

#include "main.h"


// These need to move elsewhere=======================================
#warning "These need to move elsewhere"
#define POWER_INDEX_MAX 7
extern float power[POWER_INDEX_MAX];
//float power[POWER_INDEX_MAX] = {-9.0, -5.0, 0.0, 6.0, 12.0, 18.0, 22.0};

#define MODULATION_INDEX_MAX 9
//extern const char* MY_CONFIG_NAME[MODULATION_INDEX_MAX];
extern void setModemConfig(uint8_t i);                             //need to fix
//====================================================================
#define MODULATION_INDEX_MAX 9
/*
static const char* MY_CONFIG_NAME[MODULATION_INDEX_MAX] =
{
"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"
};
*/
char modulation_array[9][20] = {"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"};
float frequency_array[] = {902.125,902.375,902.625,902.875,903.125,903.375,903.625,903.875,904.125,904.375,904.625,904.875,905.125,905.375,905.625,905.875,906.125,906.375,906.625,906.875,907.125,907.375,907.625,907.875,908.125,908.375,908.625,908.875,909.125,909.375,909.625,909.875,910.125,910.375,910.625,910.875,911.125,911.375,911.625,911.875,912.125,912.375,912.625,912.875,913.125,913.375,913.625,913.875,914.125,914.375,914.625,914.875,915.125,915.375,915.625,915.875,916.125,916.375,916.625,916.875,917.125,917.375,917.625,917.875,918.125,918.375,918.625,918.875,919.125,919.375,919.625,919.875,920.125,920.375,920.625,920.875,921.125,921.375,921.625,921.875,922.125,922.375,922.625,922.875,923.125,923.375,923.625,923.875,924.125,924.375,924.625,924.875,925.125,925.375,925.625,925.875,926.125,926.375,926.625,926.875,927.125,927.375,927.625,927.875};

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

bool is_numeric(const char* string) {
    const int string_len = strlen(string);
    for(int i = 0; i < string_len; ++i) {
      if(!isdigit(string[i])) 
        return false;
    }
    return true;
  }

  void cli_process_bool(int parameter_query, const char* param_name, char* param_command, bool* param_value ) {
    #define PRINTF_OK_BOOL "OK:%s = %s\r\n"
    #define PRINTF_NG_BOOL "NG:%s must be 'OFF' or 'ON'\r\n"
    if (parameter_query) {
        Serial.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off"); 
        telnet.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off"); 
    }
    else {
        if (strcmp(param_command, "OFF") == 0) {
            *param_value = false;
            Serial.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
            telnet.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
        }
        else if (strcmp(param_command, "ON") == 0){
            *param_value = true;
            Serial.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
            telnet.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
        }
        else {
            Serial.printf(PRINTF_NG_BOOL, param_name);
            telnet.printf(PRINTF_NG_BOOL, param_name);
        }
    }
}


void cli_process_int(int parameter_query, const char* param_name, char* param_command, int param_value_min, int param_value_max, int* param_value) {
    #define PRINTF_OK_INT "OK:%s = %u\r\n"
    #define PRINTF_NG_INT "NG:%s must be an integer >=%u and <=%u\r\n"
    int value;

    if (parameter_query) {
        Serial.printf(PRINTF_OK_INT , param_name, *param_value); 
        telnet.printf(PRINTF_OK_INT , param_name, *param_value); 
    }
    else {
            value = atoi(param_command);
        if ((value >= param_value_min && value <= param_value_max) && is_numeric(param_command)) {
            *param_value = value;
            Serial.printf(PRINTF_OK_INT, param_name, *param_value);
            telnet.printf(PRINTF_OK_INT, param_name, *param_value);
        }
        else {
            Serial.printf(PRINTF_NG_INT, param_name, param_value_min, param_value_max);
            telnet.printf(PRINTF_NG_INT, param_name, param_value_min, param_value_max);
        }   
    }
 }


 void cli_process_index_float_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, float indexed_array[], const char* param_units, int* param_index) {
    #define PRINTF_OK_FLOAT "OK:%s = %u (%3.3f %s)\r\n"
    #define PRINTF_NG_FLOAT "NG:%s must be an integer >=%u and <=%u\r\n"
    int index;

    if (parameter_query) {
        Serial.printf(PRINTF_OK_FLOAT , param_name, *param_index, indexed_array[*param_index], param_units); 
        telnet.printf(PRINTF_OK_FLOAT , param_name, *param_index, indexed_array[*param_index], param_units); 
    }
    else {
        index = atoi(param_command);
        if ((index >= param_index_min && index <= param_index_max) && is_numeric(param_command)) {
            *param_index = index;
            Serial.printf(PRINTF_OK_FLOAT, param_name, *param_index, indexed_array[*param_index], param_units);
            telnet.printf(PRINTF_OK_FLOAT, param_name, *param_index, indexed_array[*param_index], param_units);
        }
        else {
            Serial.printf(PRINTF_NG_FLOAT, param_name, param_index_min, param_index_max);
            telnet.printf(PRINTF_NG_FLOAT, param_name, param_index_min, param_index_max);
        }   
    }
 }

 void cli_process_index_char_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, char indexed_array[][20], int* param_index) {
    #define PRINTF_OK_CHAR "OK:%s = %u (%s)\r\n"
    #define PRINTF_NG_CHAR  "NG:%s must be an integer >=%u and <=%u\r\n"
    int index;

    if (parameter_query) {
        Serial.printf(PRINTF_OK_CHAR  , param_name, *param_index, indexed_array[*param_index]); 
        telnet.printf(PRINTF_OK_CHAR  , param_name, *param_index, indexed_array[*param_index]); 
    }
    else {
        index = atoi(param_command);
        if ((index >= param_index_min && index <= param_index_max) && is_numeric(param_command)) {
            *param_index = index;
            Serial.printf(PRINTF_OK_CHAR , param_name, *param_index, indexed_array[*param_index]);
            telnet.printf(PRINTF_OK_CHAR , param_name, *param_index, indexed_array[*param_index]);
        }
        else {
            Serial.printf(PRINTF_NG_CHAR , param_name, param_index_min, param_index_max);
            telnet.printf(PRINTF_NG_CHAR , param_name, param_index_min, param_index_max);
        }   
    }
 }


int cli_execute(const char* command_arg) {
    //temporary variables pending integration into ui.h
    static char  callsign[10];
    static float frequency; //depricated
    static int   frequency_index;
    static bool  beacon_state;
    static bool  gps_state;
    static float lat_value, lon_value;
    static int   modulation_index;
    static int   power_index;
    static int   tx_interval;
    static int   radio_address;
    static int   radio_type;
    static bool  serial_usb_state;
 
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
bool parameter_query;
int current_int_value;
int current_lat_value;
int current_lon_value;
char current_str_value[10];

strcpy(command, command_arg);

if (command[0] == '/') {
    upcase(command);

    cmd_code = command[1];
    command[0] = ' ';               //replace leading \ and command with blanks
    command[1] = ' ';
    removeBlanks(command);

    parameter_query = false;
    if (command[0] == '?') parameter_query = true;

    switch (cmd_code) {

//      Radio Address----------------------------------------------
    case 'A':
        current_int_value = radio_address;
        cli_process_int(parameter_query, "Radio Address", command, 0, 20 , & radio_address);
        if (current_int_value != radio_address) {
            //change radio address in the radio, save to RAM and NVRAM
        }
        break;

//      Beacon Off/On-------------------------------------------------------------

    case 'B':
        current_int_value = beacon_state;    
        cli_process_bool(parameter_query, "Beacon", command, & beacon_state);
        if (current_int_value != beacon_state){
            //change beacon state in the radio, save to RAM and NVRAM
        }
        break;
     
//      Call Sign--------------------------------------------------------------
    case 'C':
        strcpy(callsign, command); 
        strcpy(current_str_value,callsign);
        Serial.printf("OK:Call sign = %s\r\n"  , callsign);
        telnet.printf("OK:Call sign = %s\r\n", callsign);
        if (current_str_value != callsign) {
            //Change callsign in the radio, save to RAM and NVRAM
        }
        break;

//      Frequency--------------------------------------------------------------

    case 'F':
        current_int_value = frequency_index;
        cli_process_index_float_value_unit(parameter_query, "Frequency Index", command, 0, 100, frequency_array , "MHz",  &frequency_index);
        if (current_int_value != frequency_index) {
            //change frequency in the radio, save to RAM and NVRAM
            driver.setFrequency(frequency_array[frequency_index]);
        }
        break;

//      GPS Off/On-------------------------------------------------------------

    case 'G':
        current_int_value = gps_state;
        cli_process_bool(parameter_query, "GPS", command, & gps_state);
        if (current_int_value != gps_state) {
            //change GPS state in the radio, save to RAM and NVRAM
        }
        break;

//      Help-------------------------------------------------------------------
    case 'H':
        Serial.printf("Radio Address               /A <n>\r\n");
        Serial.printf("Beacon                      /B <off>|<on>\r\n");
        Serial.printf("Caallsign                   /C <callsign>\r\n");
        Serial.printf("Frequency                   /F <Frequency in MHz>\r\n");
        Serial.printf("GPS State                   /G <off>|<on>\r\n");
        Serial.printf("Help Text                   /H\r\n");
        Serial.printf("TX Interval (seconds)       /I <n>\r\n");
        Serial.printf("Position                    /L <latitude >,<longitude>\r\n");
        Serial.printf("Modulation index 0<=n<=8    /M <n>\r\n");
        Serial.printf("Power index 0<=n<=6         /P <n>\r\n");
        Serial.printf("USB Serial Output           /S <off>|<on>\r\n");
        Serial.printf("Radio Type                  /T <n>\r\n");
        Serial.printf("Commands case insensitive and blanks ignored\r\n");

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
        current_int_value = tx_interval;
        cli_process_int(parameter_query, "TX Interval", command, 10, 600 , & tx_interval);
        if (current_int_value != tx_interval) {
            //Change transmit inveral in the radio, save to RAM and NVRAM
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
                        Serial.printf("NG:Internal longitude buffer exceeded\r\n");
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
                        Serial.printf("NG:Internal latitude buffer exceeded\r\n");
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
            Serial.printf("NG:Location requires comma separation between Lat & Lon\r\n");
            telnet.printf("NG:Location requires comma separation between Lat & Lon\r\n");
            return 1;
        }
        lon_value = atof(lon_str);
        lat_value = atof(lat_str);

        if (abs(lon_value) > 180) {
            Serial.printf("NG:Longitude must be between -180 and 180\r\n");
            telnet.printf("NG:Longitude must be between -180 and 180\r\n");
            return 1;
        }
        if (abs(lat_value) > 90) {
            Serial.printf("NG:Longitude must be between -90 and 90\r\n");
            telnet.printf("NG:Longitude must be between -90 and 90\r\n");
            return 1;
        }
        if ((current_lat_value != lat_value) || (current_lon_value != lon_value)) {
                //Save lat/lon pair to RAM and NVRAM
        }
        Serial.printf("OK:Latitude = % f; Longitude = %f\n"  , lat_value, lon_value);
        telnet.printf("OK:Latitude = % f; Longitude = %f\r\n", lat_value, lon_value);

        break;

//      Modulation-------------------------------------------------------------
        case 'M':
            current_int_value = modulation_index;
            cli_process_index_char_value_unit(parameter_query, "Modulation Index", command, 0, 8, modulation_array,  &modulation_index);
            if (current_int_value != modulation_index) {
                //Change modulation index in the radio, save to RAM and NVRAM
            }
            break;

//      Power------------------------------------------------------------------
        case 'P':
            current_int_value = power_index;
            cli_process_index_float_value_unit(parameter_query, "Power Index", command, 0, 6, power , "dBm",  &power_index);
            if (current_int_value != power_index) {
                //Change power index in the radio, save to RAM and NVRAM
                driver.setTxPower(power[power_index]);
            }
            break;

    //      Quit Telnet session----------------------------------------
        case 'Q':
            Serial.printf("OK:Quitting Telnet session\r\n");
            telnet.printf("OK:Quitting Telnet session\r\n");
            break;

//      Serial USB Output Off/On-----------------------------------

        case 'S':
            current_int_value = serial_usb_state;
            cli_process_bool(parameter_query, "Serial USB Output", command, & serial_usb_state);
            if (current_int_value != serial_usb_state) {
                //Change serial USB state in the radio, save to RAM and NVRAM
            }
            break;

//      Radio Type-----------------------------------------------------
        case 'T':
            current_int_value = radio_type;
            cli_process_int(parameter_query, "Radio Type", command, 0, 2 , & radio_type);
            if (current_int_value != radio_type) {
                //Change radio type in the radio, save to RAM and NVRAM
            }
             break;

//      Invalid Command--------------------------------------------------------
        default:
            Serial.printf("NG:Unrecognized command %c [C, F, G, H, I, L, M, P, R]\r\n"  , cmd_code);
            telnet.printf("NG:Unrecognized command %c [C, F, G, H, I, L, M, P, R]\r\n", cmd_code);
        }
    }
    else {
        Serial.printf("NG:Leading backslash missing\r\n");
        telnet.printf("NG:Leading backslash missing\r\n");
        return 1;
    }

    return 0;
}


