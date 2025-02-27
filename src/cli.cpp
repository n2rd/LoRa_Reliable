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

    Radio Address           /A                                              Default = 0
    Beacon Disable and      /B <OFF|ON>                                     Default = OFF
    TX Lockout
    Callsign				/C <call>
    Default Config          /D
    Frequency index         /F <n>                                          Default = 905.2
                                    n= Frequency in MHz 3.g., 905.2, 
    GPS                     /G <OFF|TX|ON>                                  Default = OFF
                                    
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
    RF Signal Reports (CSV) /R <OFF|ON>                                     Default = TBD 
    Serial USB Output       /S <OFF|ON>                                     Default = on
    Radio Type              /T <n>                                          Default = 2
                                    n=0 Server (client/server operation)
                                    n=1 Client (client/server operation)
                                    n=2 Peer (peer to peer operation)
    Write NVRAM             /W
    Grid                    /X      4 or 6 character maidenhead grid square
    Short Pause             /Y <OFF|ON>                                     Default = OFF      
*/

#include "main.h"


// These need to move elsewhere=======================================
#warning "These need to move elsewhere"
/*
uint16_t encode_grid4(String locator) {
    return ((locator[0] - 'A') * 18 + (locator[1] - 'A') )* 100 + (locator[2] - '0') * 10 + (locator[3] - '0') ;
  }
  
  void decode_grid4(uint16_t grid4, char *grid) {
    grid[0] = (grid4 / 180) + 'A';
    grid[1] = (grid4 % 180) / 10 + 'A';
    grid[2] = (grid4 % 100) / 10 + '0';
    grid[3] = (grid4 % 10) + '0';
    grid[4] = '\0';
  }
*/

//====================================================================
#define MODULATION_INDEX_MAX 9


char gps_array[3][4] = {"Off", "TX", "On"};
char modulation_array[9][20] = {"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"};
float frequency_array[] = {902.125,902.375,902.625,902.875,903.125,903.375,903.625,903.875,904.125,904.375,904.625,904.875,905.125,905.375,905.625,905.875,906.125,906.375,906.625,906.875,907.125,907.375,907.625,907.875,908.125,908.375,908.625,908.875,909.125,909.375,909.625,909.875,910.125,910.375,910.625,910.875,911.125,911.375,911.625,911.875,912.125,912.375,912.625,912.875,913.125,913.375,913.625,913.875,914.125,914.375,914.625,914.875,915.125,915.375,915.625,915.875,916.125,916.375,916.625,916.875,917.125,917.375,917.625,917.875,918.125,918.375,918.625,918.875,919.125,919.375,919.625,919.875,920.125,920.375,920.625,920.875,921.125,921.375,921.625,921.875,922.125,922.375,922.625,922.875,923.125,923.375,923.625,923.875,924.125,924.375,924.625,924.875,925.125,925.375,925.625,925.875,926.125,926.375,926.625,926.875,927.125,927.375,927.625,927.875};

//000    	001    	002    	003    	004    	005    	006    	007    	008    	009    	010    	011    	012    	013    	014    	015    	016    	017    	018    	019    	020    	021    	022    	023    	024    	025    	026    	027    	028    	029    	030    	031    	032    	033    	034    	035    	036    	037    	038    	039    	040    	041    	042    	043    	044    	045    	046    	047    	048    	049    	050    	051    	052    	053    	054    	055    	056    	057    	058    	059    	060    	061    	062    	063    	064    	065    	066    	067    	068    	069    	070    	071    	072    	073    	074    	075    	076    	077    	078    	079    	080    	081    	082    	083    	084    	085    	086    	087    	088    	089    	090    	091    	092    	093    	094    	095    	096    	097    	098    	099    	100    	101    	102    	103    
//902.125	902.375	902.625	902.875	903.125	903.375	903.625	903.875	904.125	904.375	904.625	904.875	905.125	905.375	905.625	905.875	906.125	906.375	906.625	906.875	907.125	907.375	907.625	907.875	908.125	908.375	908.625	908.875	909.125	909.375	909.625	909.875	910.125	910.375	910.625	910.875	911.125	911.375	911.625	911.875	912.125	912.375	912.625	912.875	913.125	913.375	913.625	913.875	914.125	914.375	914.625	914.875	915.125	915.375	915.625	915.875	916.125	916.375	916.625	916.875	917.125	917.375	917.625	917.875	918.125	918.375	918.625	918.875	919.125	919.375	919.625	919.875	920.125	920.375	920.625	920.875	921.125	921.375	921.625	921.875	922.125	922.375	922.625	922.875	923.125	923.375	923.625	923.875	924.125	924.375	924.625	924.875	925.125	925.375	925.625	925.875	926.125	926.375	926.625	926.875	927.125	927.375	927.625	927.875

//

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

  bool is_float(const char* string) {
    int period_counter;
    const int string_len = strlen(string);
    period_counter = 0;
    for(int i = 0; i < string_len; ++i) {
        if(!isdigit(string[i])) {
            if (string[i] != '.') {
                return false;
            }
            else {
                if (++period_counter>1) return false;
            }
        }   
    }
    return true;
  }
 

void cli_process_bool(int parameter_query, const char* param_name, char* param_command, bool* param_value ) {
    #define PRINTF_OK_BOOL "OK:%s = %s\r\n"
    #define PRINTF_NG_BOOL "NG:%s must be 'OFF' or 'ON'\r\n"
    if (parameter_query) {
        ps_st.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off"); 
    }
    else {
        if (strcmp(param_command, "OFF") == 0) {
            *param_value = false;
            ps_st.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
        }
        else if (strcmp(param_command, "ON") == 0){
            *param_value = true;
            ps_st.printf(PRINTF_OK_BOOL , param_name, *param_value ? "On" : "Off");
        }
        else {
            ps_st.printf(PRINTF_NG_BOOL, param_name);
        }
    }
}

void cli_process_int(int parameter_query, const char* param_name, char* param_command, int param_value_min, int param_value_max, int* param_value) {
    #define PRINTF_OK_INT "OK:%s = %u\r\n"
    #define PRINTF_NG_INT "NG:%s must be an integer >=%u and <=%u\r\n"
    int value;

    if (parameter_query) {
        ps_st.printf(PRINTF_OK_INT , param_name, *param_value); 
    }
    else {
            value = atoi(param_command);
        if ((value >= param_value_min && value <= param_value_max) && is_numeric(param_command)) {
            *param_value = value;
            ps_st.printf(PRINTF_OK_INT, param_name, *param_value);
        }
        else {
            ps_st.printf(PRINTF_NG_INT, param_name, param_value_min, param_value_max);
        }   
    }
}


 void cli_process_index_float_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, float indexed_array[], const char* param_units, int* param_index) {
    #define PRINTF_OK_FLOAT "OK:%s = %u (%3.3f %s)\r\n"
    #define PRINTF_NG_FLOAT "NG:%s must be an integer >=%u and <=%u\r\n"
    int index;

    if (parameter_query) {
        ps_st.printf(PRINTF_OK_FLOAT , param_name, *param_index, indexed_array[*param_index], param_units); 
    }
    else {
        index = atoi(param_command);
        if ((index >= param_index_min && index <= param_index_max) && is_numeric(param_command)) {
            *param_index = index;
            ps_st.printf(PRINTF_OK_FLOAT, param_name, *param_index, indexed_array[*param_index], param_units);
        }
        else {
            ps_st.printf(PRINTF_NG_FLOAT, param_name, param_index_min, param_index_max);
        }   
    }
 }

 void cli_process_index_char_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, char indexed_array[][20], int* param_index) {
    #define PRINTF_OK_CHAR  "OK:%s = %u (%s)\r\n"
    #define PRINTF_NG_CHAR  "NG:%s must be an integer >=%u and <=%u\r\n"
    int index;

    if (parameter_query) {
        ps_st.printf(PRINTF_OK_CHAR  , param_name, *param_index, indexed_array[*param_index]); 
    }
    else {
        index = atoi(param_command);
        if ((index >= param_index_min && index <= param_index_max) && is_numeric(param_command)) {
            *param_index = index;
            ps_st.printf(PRINTF_OK_CHAR , param_name, *param_index, indexed_array[*param_index]);
        }
        else {
            ps_st.printf(PRINTF_NG_CHAR , param_name, param_index_min, param_index_max);
        }   
    }
 }


int cli_execute(const char* command_arg) {

static bool local_PARMS_parameters_csv_output;

PARAMETERS local_params;

#define PRINTF_NG_NULL_ARG "NG:Arguement required and none provided\r\n"


static char     callsign[10];
static float    frequency; //depricated
static int      frequency_index;
static bool     tx_lock_state;
static bool     short_pause_state;
static bool     csv_output_state;
static bool     gps_state;
static int      gps_index;
static float    lat_value, lon_value;
static int      modulation_index;
static int      power_index;
static int      tx_interval;
static int      radio_address;
static int      radio_type;
static bool     serial_usb_state;
static uint16_t grid4;          // (2B) 4 char grid square, encoded from 0 to 32,399
static char     grid5;          // (1B) 1 char subsquare identifier, encoded as an ascii char 
static char     grid6;          // (1B) 1 char subsquare identifier, encoded as an ascii char 

char        command[50];
char        cmd_code;
char*       param_str[50];
char        lat_str[20], lon_str[20];
char        *lat_str_ptr = lat_str;
char        *lon_str_ptr = lon_str;
float       lat_sign,lon_sign;
int         i, j;
int         int_input;
float       flt_input;
int         status;
bool        location_comma_found;
int         lat_i, lon_i;
bool        parameter_query;
int         current_int_value;
int         current_lat_value;
int         current_lon_value;
char        current_str_value[10];
uint16_t    current_uint16_value;
char        current_char_value1;
char        current_char_value2;
bool        valid_gridsquare_format;
char        grid4_str_value[5];


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

//      Radio Address----------------------------------------------------------
    case 'A':
        current_int_value = PARMS.parameters.address;
        radio_address     = PARMS.parameters.address;
        cli_process_int(parameter_query, "Radio Address", command, 1, 254 , & radio_address);
        if (current_int_value != radio_address) {
            PARMS.parameters.address = radio_address;
            manager.setThisAddress(radio_address);
            driver.setHeaderFrom(radio_address);
        }
        break;

//      TX Lock (Beacon disable) Off/On----------------------------------------

    case 'B':
        current_int_value  = PARMS.parameters.tx_lock;    
        tx_lock_state      = PARMS.parameters.tx_lock;
        cli_process_bool(parameter_query, "TX Lock (Beacon Disable)", command, & tx_lock_state);
        if (current_int_value != tx_lock_state){
            PARMS.parameters.tx_lock = tx_lock_state;
        }
        break;
     
//      Call Sign--------------------------------------------------------------
    case 'C':
        if (command[0] != '\0') {
            strcpy(callsign, PARMS.parameters.callsign);
            if (parameter_query) {
                ps_st.printf("OK:Call sign = %s\r\n", callsign);
            }
            else {
                strcpy(callsign, command); 
                strcpy(current_str_value,callsign);
                ps_st.printf("OK:Call sign = %s\r\n", callsign);
                if (current_str_value != callsign) {
                    strcpy(PARMS.parameters.callsign, callsign);
                }    
            }
        }
        else {
            ps_st.printf(PRINTF_NG_NULL_ARG);
        }
        break;
//      Reset radio to default state-------------------------------------------
    case 'D':


       break;

//      Frequency--------------------------------------------------------------
    case 'F':
        if (command[0] != '\0') {
            current_int_value = PARMS.parameters.frequency_index;
            frequency_index   = PARMS.parameters.frequency_index;
            cli_process_index_float_value_unit(parameter_query, "Frequency Index", command, 0, sizeof(frequency_array)/sizeof(frequency_array[0])-1, frequency_array , "MHz",  &frequency_index);
            if (current_int_value != frequency_index) {
                PARMS.parameters.frequency_index = frequency_index;
                driver.setFrequency(frequency_array[frequency_index]);
            }
        }
        else {
            ps_st.printf(PRINTF_NG_NULL_ARG);
        }
         break;

//      GPS Off/GPS on on Transmit only/On-------------------------------------
    case 'G':
        #if HAS_GPS == 1
            if (command[0] != '\0') {
                #define PRINTF_OK_GPS "OK:%u (%s)\r\n"
                #define PRINTF_NG_GPS "NG:%u must be 0 for OFF, 1 for ON AT TX or 2 for ON\r\n"
    
    
                current_int_value = PARMS.parameters.gps_state;
                gps_index         = PARMS.parameters.gps_state;
                if (parameter_query) {
                    //ps_st.printf(PRINTF_OK_GPS, current_int_value, gps_power_state_name[current_int_value]);
                    ps_st.printf(PRINTF_OK_GPS, current_int_value, GPS.getPowerStateName((GPSClass::PowerState) current_int_value));
                }
                else {
                    int_input = atoi(command);
                    if ((int_input >= 0 && int_input <= 2) && is_numeric(command)) {
                        PARMS.parameters.gps_state = int_input;
                        ps_st.printf(PRINTF_OK_GPS, (int)PARMS.parameters.gps_state, GPS.getPowerStateName((GPSClass::PowerState) int_input));
                    }
                    else {
                        ps_st.printf(PRINTF_NG_GPS, int_input);
                    }
                }
    
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
        #else //HAS_GPS == 0
            ps_st.printf("NO GPS defined for this device\r\n");
        #endif
        break;

//      Help-------------------------------------------------------------------
    case 'H':
    case '?':
        ps_st.printf("Radio Address                    /A <n>\r\n");
        ps_st.printf("Beacon Disable (TX Lockout)      /B <off>|<on>\r\n");
        ps_st.printf("Caallsign                        /C <callsign>\r\n");
        ps_st.printf("Reset radio to default state     /D\r\n");        
        ps_st.printf("Frequency                        /F <Frequency in MHz>\r\n");
        ps_st.printf("GPS State                        /G 0 for OFF, 1 for ON FOR TX and 2 for ON<off>|<tx<on>\r\n");
        ps_st.printf("Help Text                        /H\r\n");
        ps_st.printf("TX Interval (seconds)            /I <n>\r\n");
        ps_st.printf("Position                         /L <latitude >,<longitude>\r\n");
        ps_st.printf("Modulation index 0<=n<=8         /M <n>\r\n");
        ps_st.printf("Power index 0<=n<=6              /P <n>\r\n");
        ps_st.printf("USB Serial Output                /S <off>|<on>\r\n");
        ps_st.printf("Radio Type                       /T <n>\r\n");
        ps_st.printf("Write no NVRAM                   /W\r\n");
        ps_st.printf("Maidenhead grid square (4 or 6)  /X\r\n");
        ps_st.printf("Short TX Pause                   /Y <off>|<on>\r\n");
        ps_st.printf("CSV Output                       /Z <off>|<on>\r\n");
        ps_st.printf("Commands case insensitive and blanks ignored\r\n");
        break;

//      Interval (transmit)----------------------------------------------------

    case 'I':
        current_int_value = PARMS.parameters.tx_interval;
        tx_interval       = PARMS.parameters.tx_interval;
        cli_process_int(parameter_query, PARMS.Key.tx_interval, command, 10, 255 , & tx_interval);
        if (current_int_value != tx_interval) {
            PARMS.parameters.tx_interval = tx_interval;
        }
        break;

//      Location---------------------------------------------------------------
    case 'L':
        if (parameter_query) {
            lat_value = PARMS.parameters.lat_value;
            lon_value = PARMS.parameters.lon_value;
            ps_st.printf("OK:Latitude = % f; Longitude = %f\r\n", lat_value, lon_value);
        }
        else {
            location_comma_found = false;
            i = 0;
            lat_i = 0;
            lon_i = 0;
            lat_str[0] = '\0';
            lon_str[0] = '\0';
            lat_sign = 1;
            lon_sign = 1;
            while (command[i] != '\0') {
                if (command[i] != ',') {
                    if (location_comma_found) {
                        if (lon_i < sizeof(lon_str)-1) {
                            lon_str[lon_i] = command[i];
                            lon_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal longitude buffer exceeded\r\n");
                            return 1;
                        }
                    }
                    else {
                        if (lat_i < sizeof(lat_str)-1) {
                            lat_str[lat_i] = command[i];
                            lat_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal latitude buffer exceeded\r\n");
                            return 1;
                        }
                    }
                }
                else {
                        location_comma_found = true;
                }
                i++;
            }
            if (lat_str[0] == '-') {
                lat_str_ptr++;
                lat_sign = -1;
            }
            if (lon_str[0] == '-') {
                lon_str_ptr++;
                lon_sign = -1;
            }
            lat_str[lat_i] = '\0';
            lon_str[lon_i] = '\0';
    
            if (!location_comma_found) {
                ps_st.printf("NG:Location requires comma separation between Lat & Lon\r\n");
                return 1;
            }
            if (!(is_float(lon_str_ptr) && is_float(lat_str_ptr))) {
                ps_st.printf("NG:Latitude and Longitude must be valid floating point numbers\r\n");
                return 1;
            }
            lon_value = atof(lon_str_ptr) * lon_sign;
            lat_value = atof(lat_str_ptr) * lat_sign;
    
            if (abs(lon_value) > 180) {
                ps_st.printf("NG:Longitude must be between -180 and 180\r\n");
                return 1;
            }
            if (abs(lat_value) > 90) {
                ps_st.printf("NG:Longitude must be between -90 and 90\r\n");
                return 1;
            }
            if ((current_lat_value != lat_value) || (current_lon_value != lon_value)) {
                    PARMS.parameters.lat_value = lat_value;
                    PARMS.parameters.lon_value = lon_value;    
            }
            ps_st.printf("OK:Latitude = % f; Longitude = %f\r\n", lat_value, lon_value);
        }

        break;

//      Modulation-------------------------------------------------------------
        case 'M':
            if (command[0] != '\0') {
                current_int_value = PARMS.parameters.modulation_index;
                modulation_index  = PARMS.parameters.modulation_index;
                cli_process_index_char_value_unit(parameter_query, "Modulation Index", command, 0, sizeof(modulation_array)/sizeof(modulation_array[0])-1, modulation_array,  &modulation_index);
                if (current_int_value != modulation_index) {
                    //Change modulation index in the radio, save to RAM and NVRAM
                    PARMS.parameters.modulation_index = modulation_index;
                    setModemConfig(modulation_index); //SF Bandwith etc
                }
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
            break;

//      Power------------------------------------------------------------------
        case 'P':
            if (command[0] != '\0') {
                current_int_value = PARMS.parameters.power_index;
                power_index       = PARMS.parameters.power_index;
                cli_process_index_float_value_unit(parameter_query, "Power Index", command, 0, sizeof(power)/sizeof(power[0])-1, power , "dBm",  &power_index);
                if (current_int_value != power_index) {
                    //Change power index in the radio, save to RAM and NVRAM
                    PARMS.parameters.power_index = power_index;
                    driver.setTxPower(power[power_index]);
                }
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
            break;

//  Quit Telnet session----------------------------------------
        case 'Q':
            ps_st.printf("OK:Quitting Telnet session\r\n");
            break;

//      Serial USB Output Off/On-----------------------------------

        case 'S':
            current_int_value = serial_usb_state;
            cli_process_bool(parameter_query, "Serial USB Output", command, & serial_usb_state);
            if (current_int_value != serial_usb_state) {
            }
            break;

//      Radio Type-------------------------------------------------------------
        case 'T':
            if (command[0] != '\0')
            {
                //current_int_value = PARMS.parameters.radio_type;
                radio_type        = PARMS.parameters.radioType;
                current_int_value = radio_type;
                cli_process_int(parameter_query, "Radio Type", command, 0, 2 , & radio_type);
                if (current_int_value != radio_type) {
                    PARMS.parameters.radioType = radio_type;
                }
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
             }
            break;

//      Write to NVRAM---------------------------------------------------------
        case 'W':
            PARMS.update(); // update current settings in parameters structure to nvs
            ps_st.printf("OK: Non-volatile Storage updated with current values\r\n"); 
            break;
    
//      Maidenhead Grid Square (4 or 6 characters)------------------------------
/*
            Character pairs encode longitude first, and then latitude.
            The first pair (a field) encodes with base 18 and the letters "A" to "R".
            The second pair (square) encodes with base 10 and the digits "0" to "9".
            The third pair (subsquare) encodes with base 24 and the letters "A" to "X".
            The fourth pair (extended square) encodes with base 10 and the digits "0" to "9".
    
            uint16_t grid4;  // (2B) 4 char grid square, encoded from 0 to 32,399
            char     grid5;  // (1B) 1 char subsquare identifier, encoded as an ascii char 
            char     grid6; // (1B) 1 char subsquare identifier, encoded as an ascii char 
*/
        case 'X':
            if (command[0] != '\0')
            {
                if (parameter_query){
                    current_uint16_value = PARMS.parameters.grid4;
                    decode_grid4(current_uint16_value, current_str_value);
                    current_char_value1  = PARMS.parameters.grid5;
                    current_char_value2  = PARMS.parameters.grid6;
                    ps_st.printf("OK:Grid4=%s (%u). Grid5=%c, Grid6=%c\r\n", current_str_value, (unsigned int)current_uint16_value, current_char_value1, current_char_value2);
                }
                else {
                    i = strlen(command);
                    if (i == 4) {
                        valid_gridsquare_format =       ((int)'A' <= (int)command[0] && (int)command[0] <= (int('R'))) &&
                                                        ((int)'A' <= (int)command[1] && (int)command[1] <= (int('R'))) &&
                                                        ((int)'0' <= (int)command[2] && (int)command[2] <= (int('9'))) &&
                                                        ((int)'0' <= (int)command[3] && (int)command[3] <= (int('9'))) ;
                        if (valid_gridsquare_format) {
                            strcpy(current_str_value, command);
                            PARMS.parameters.grid4 = encode_grid4(command);
                            PARMS.parameters.grid5 = 'l';       //L for mid grid location
                            PARMS.parameters.grid6 = 'l';       //L for mid grid location
                        }
                        else {
                            ps_st.printf("NG:Invalid 4 character grid\r\n");
                            return 1;
                        }
                    }
                    else if (i == 6) {
                            valid_gridsquare_format =   ((int)'A' <= (int)command[0] && (int)command[0] <= (int('R'))) &&
                                                        ((int)'A' <= (int)command[1] && (int)command[1] <= (int('R'))) &&
                                                        ((int)'0' <= (int)command[2] && (int)command[2] <= (int('9'))) &&
                                                        ((int)'0' <= (int)command[3] && (int)command[3] <= (int('9'))) &&
                                                        ((int)'A' <= (int)command[4] && (int)command[4] <= (int('X'))) &&
                                                        ((int)'A' <= (int)command[5] && (int)command[5] <= (int('X')));
                        if (valid_gridsquare_format) {
                            for (j = 0; j < 4; j++) {
                                grid4_str_value[j]   = command[j];
                                current_str_value[j] = command[j];
                            }
                            grid4_str_value[4]   = '\0';
                            current_str_value[4] = '\0';
                            PARMS.parameters.grid4 = encode_grid4(command);
                            PARMS.parameters.grid5 = command[4];
                            PARMS.parameters.grid6 = command[5];
                        }
                        else {
                            ps_st.printf("NG:Invalid 6 character grid\r\n");
                            return 1;
                        }
                    }
                    else {
                        ps_st.printf("NG:Gridsquare must be 4 or 6 character gridswuare\r\n");
                        return 1;
                    }
                    current_uint16_value = PARMS.parameters.grid4;
                    current_char_value1  = PARMS.parameters.grid5;
                    current_char_value2  = PARMS.parameters.grid6;
                    ps_st.printf("OK:Grid4=%s (%u). Grid5=%c, Grid6=%c\r\n", current_str_value, (unsigned int)current_uint16_value, current_char_value1, current_char_value2);
                }
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
        
            break;

//      Short TX Pause Off/On--------------------------------------------------

        case 'Y':
            current_int_value = PARMS.parameters.short_pause;    
            short_pause_state = PARMS.parameters.short_pause;
            cli_process_bool(parameter_query, "Short Pause State", command, & short_pause_state);
            if (current_int_value != short_pause_state){
                PARMS.parameters.short_pause = short_pause_state;
            }

        break;

//      CSV Output Off/On------------------------------------------------------

        case 'Z':
            current_int_value = local_PARMS_parameters_csv_output;    
            csv_output_state  = local_PARMS_parameters_csv_output;
            cli_process_bool(parameter_query, "CSV Output State", command, & csv_output_state);
            if (current_int_value != csv_output_state){
                local_PARMS_parameters_csv_output = csv_output_state;
            }

break;

//      Invalid Command--------------------------------------------------------
        default:
            ps_st.printf("NG:Unrecognized command %c [C, F, G, H, I, L, M, P, R]\r\n", cmd_code);
        }
    }
    else {
        ps_st.printf("NG:Leading backslash missing\r\n");
        return 1;
    }

    return 0;
}


