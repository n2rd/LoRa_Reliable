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

    Wifi Credentials        /@ SSID, passcode
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
    RF Signal Reports (CSV) /R <n>                                          Default = 3
                                    n=0  OFF
                                    n=1  SERIAL
                                    n=2  TELNET
                                    n=3  BOTH    
    Radio Type              /T <n>                                          Default = 2
                                    n=0 Server (client/server operation)
                                    n=1 Client (client/server operation)
                                    n=2 Peer (peer to peer operation)
    
                                    Version                 /V                                
    
    Promiscuous Mode        /U  <OFF|ON>                                    Default = OFF
                                Note: Special compile required

                                    Write NVRAM             /W
    Grid                    /X      4 or 6 character maidenhead grid square
    Short Pause             /Y <OFF|ON>                                     Default = OFF      
*/

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
#include "main.h"

#define MODULATION_INDEX_MAX 9

//char gps_array[3][4] = {"Off", "TX", "On"};
char csv_array[4][20] = {"OFF", "SERIAL", "TELNET", "BOTH"};
static u_int8_t  csv_index;
char modulation_array[9][20] = {"Short Turbo", "Short Fast", "Short Slow", "Medium Fast", "Medium Slow", "Long Fast", "Long Moderate", "Long Slow", "Very Long Slow"};
float frequency_array[] = {902.125,902.375,902.625,902.875,903.125,903.375,903.625,903.875,904.125,904.375,904.625,904.875,905.125,905.375,905.625,905.875,906.125,906.375,906.625,906.875,907.125,907.375,907.625,907.875,908.125,908.375,908.625,908.875,909.125,909.375,909.625,909.875,910.125,910.375,910.625,910.875,911.125,911.375,911.625,911.875,912.125,912.375,912.625,912.875,913.125,913.375,913.625,913.875,914.125,914.375,914.625,914.875,915.125,915.375,915.625,915.875,916.125,916.375,916.625,916.875,917.125,917.375,917.625,917.875,918.125,918.375,918.625,918.875,919.125,919.375,919.625,919.875,920.125,920.375,920.625,920.875,921.125,921.375,921.625,921.875,922.125,922.375,922.625,922.875,923.125,923.375,923.625,923.875,924.125,924.375,924.625,924.875,925.125,925.375,925.625,925.875,926.125,926.375,926.625,926.875,927.125,927.375,927.625,927.875};

//000    	001    	002    	003    	004    	005    	006    	007    	008    	009    	010    	011    	012    	013    	014    	015    	016    	017    	018    	019    	020    	021    	022    	023    	024    	025    	026    	027    	028    	029    	030    	031    	032    	033    	034    	035    	036    	037    	038    	039    	040    	041    	042    	043    	044    	045    	046    	047    	048    	049    	050    	051    	052    	053    	054    	055    	056    	057    	058    	059    	060    	061    	062    	063    	064    	065    	066    	067    	068    	069    	070    	071    	072    	073    	074    	075    	076    	077    	078    	079    	080    	081    	082    	083    	084    	085    	086    	087    	088    	089    	090    	091    	092    	093    	094    	095    	096    	097    	098    	099    	100    	101    	102    	103    
//902.125	902.375	902.625	902.875	903.125	903.375	903.625	903.875	904.125	904.375	904.625	904.875	905.125	905.375	905.625	905.875	906.125	906.375	906.625	906.875	907.125	907.375	907.625	907.875	908.125	908.375	908.625	908.875	909.125	909.375	909.625	909.875	910.125	910.375	910.625	910.875	911.125	911.375	911.625	911.875	912.125	912.375	912.625	912.875	913.125	913.375	913.625	913.875	914.125	914.375	914.625	914.875	915.125	915.375	915.625	915.875	916.125	916.375	916.625	916.875	917.125	917.375	917.625	917.875	918.125	918.375	918.625	918.875	919.125	919.375	919.625	919.875	920.125	920.375	920.625	920.875	921.125	921.375	921.625	921.875	922.125	922.375	922.625	922.875	923.125	923.375	923.625	923.875	924.125	924.375	924.625	924.875	925.125	925.375	925.625	925.875	926.125	926.375	926.625	926.875	927.125	927.375	927.625	927.875

//

//=============================================================================
void upcase(char* str) {

    int i;

    i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}


//=============================================================================
void removeBlanks(char* str) {
    int i, j = 0;

    for (i = 0; str[i] != '\0'; i++) {
        if (!isspace(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

//=============================================================================
bool is_numeric(const char* string) {
    const int string_len = strlen(string);
    for(int i = 0; i < string_len; ++i) {
      if(!isdigit(string[i])) 
        return false;
    }
    return true;
  }

//=============================================================================
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
 

//=============================================================================
//void cli_process_bool(int parameter_query, const char* param_name, char* param_command, bool* param_value ) {
void cli_process_bool(int parameter_query, const char* param_name, char* param_command, u_int8_t* param_value ) {
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

//=============================================================================
void cli_process_int(int parameter_query, const char* param_name, char* param_command, int param_value_min, int param_value_max, u_int8_t* param_value) {
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


//=============================================================================
void cli_process_index_float_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, float indexed_array[], const char* param_units, u_int8_t* param_index) {
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

//=============================================================================
void cli_process_index_char_value_unit(int parameter_query, const char* param_name, char* param_command, int param_index_min, int param_index_max, char indexed_array[][20], u_int8_t *param_index) {
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
 
//=============================================================================
int cli_execute_command(const char* command_arg) {
//int cli_execute(const char* command_arg) {

static u_int8_t local_PARMS_parameters_csv_output;
static u_int8_t local_promiscuous_mode;

PARAMETERS local_params;

//=======================================================
static u_int8_t csv_current_state;
static char ssid_str[33], passcode_str[64];
//=======================================================

#define PRINTF_NG_NULL_ARG "NG:Arguement required and none provided\r\n"

int         ssid_i, passcode_i;
u_int8_t   current_state;

char        command[50],command_original_case[50];
char        cmd_code;
char*       param_str[50];
char        lat_str[20], lon_str[20];

char        *lat_str_ptr = lat_str;
char        *lon_str_ptr = lon_str;
float       lat_sign,lon_sign;
char        str_grid4_value[5];
int         i, j;
int         int_input;
float       flt_input;
int         status;
bool        comma_found;
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
strcpy(command_original_case, command_arg);     //preserve case for WiFi credentials

if (command[0] == '/') {
    upcase(command);

    cmd_code = command[1];
    command[0] = ' ';               //replace leading \ and command with blanks
    command[1] = ' ';
    removeBlanks(command);

    parameter_query = false;
    if (command[0] == '?') parameter_query = true;

    switch (cmd_code) {

//      Wifi Credentials----------------------------------------------------------
/*
These SSIDs can be zero to 32 octets (32 bytes) long,[11] and are, for convenience, 
usually in a natural language, such as English. The 802.11 standards prior to the 
2012 edition did not define any particular encoding or representation for SSIDs, 
hich were expected to be treated and handled as an arbitrary sequence of 0–32 octets 
that are not limited to printable characters. IEEE Std 802.11-2012 defines a flag to 
express that the SSID is UTF-8-encoded and could contain any Unicode text.[12] Wireless 
network stacks must still be prepared to handle arbitrary values in the SSID field.

1. A password is 8–63 characters long. A password of 14 or 15 characters would be 
   long enough to defeat most brute-force guessing.
2. Avoid Trailing and leading spaces (ASCII 0x20)
3. Be careful with certain special characters, such as # % & " $ ￡
*/
    case '@':
        if (parameter_query) {
            ps_st.printf("OK:SSID = \"%s\"; Passcode = \"%s\"\r\n",
                PARMS.parameters.wifiSSID,
                "************"
                );
        }
        #if defined(USE_WIFI) && (USE_WIFI >0)
        else {
			command_original_case[0] = ' ';               //replace leading \ and command with blanks
			command_original_case[1] = ' ';
			removeBlanks(command_original_case);
			
            comma_found = false;
            ssid_i     = 0;
            passcode_i = 0;
            ssid_str[0]     = '\0';
            passcode_str[0] = '\0';
            while (command_original_case[i] != '\0') {
                if (command_original_case[i] != ',') {
                    if (!comma_found) {
                        if (ssid_i < sizeof(ssid_str)-1) {
                            ssid_str[ssid_i] = command_original_case[i];
                            ssid_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal WiFi SSID buffer exceeded\r\n");
                            return 1;
                        }
                    }
                    else {
                        if (passcode_i < sizeof(passcode_str)-1) {
                            passcode_str[passcode_i] = command_original_case[i];
                            passcode_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal WiFi passcode buffer exceeded\r\n");
                            return 1;
                        }
                   }
                }
                else {
                        comma_found = true;
                }
                i++;
            }
            ssid_str[ssid_i]         = '\0';
            passcode_str[passcode_i] = '\0';
    
            if (!comma_found) {
                ps_st.printf("NG:WiFi SSID and Passoode requires comma separation between SSID & Passcode\r\n");
                return 1;
            }
            ps_st.printf("OK:SSID = \"%s\"; Passcode = \"%s\"\r\n", ssid_str, passcode_str);
            strcpy(PARMS.parameters.wifiSSID, ssid_str);
            strcpy(PARMS.parameters.wifiKey, passcode_str);
            if (WIFI.changeAP()) {
                initializeNetwork();
            }
            else {
                /* should we do anything in the case of failure */
            }
        }
       #endif
       break;

//      Radio Address----------------------------------------------------------
    case 'A':
        local_params.address = PARMS.parameters.address;
        cli_process_int(parameter_query, "Radio Address", command, 1, 254 , & local_params.address);
        PARMS.parameters.address = local_params.address;
        manager.setThisAddress(local_params.address);
        driver.setHeaderFrom(local_params.address);
        break;

//      TX Lock (Beacon disable) Off/On----------------------------------------

    case 'B':
        local_params.tx_lock      = PARMS.parameters.tx_lock;
        cli_process_bool(parameter_query, "TX Lock (Beacon Disable)", command, & local_params.tx_lock);
        PARMS.parameters.tx_lock = local_params.tx_lock;
        break;
     
//      Call Sign--------------------------------------------------------------
    case 'C':
        if (command[0] != '\0') {
            strcpy(local_params.callsign, PARMS.parameters.callsign);
            if (parameter_query) {
                ps_st.printf("OK:Call sign = %s\r\n", local_params.callsign);
            }
            else {
                strcpy(local_params.callsign, command); 
                ps_st.printf("OK:Call sign = %s\r\n", local_params.callsign);
                strcpy(PARMS.parameters.callsign, local_params.callsign);
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
            local_params.frequency_index   = PARMS.parameters.frequency_index;
            cli_process_index_float_value_unit(parameter_query, "Frequency", command, 0, sizeof(frequency_array)/sizeof(frequency_array[0])-1, frequency_array , "MHz",  & local_params.frequency_index);
            PARMS.parameters.frequency_index = local_params.frequency_index;
            driver.setFrequency(frequency_array[local_params.frequency_index]);
        }
        else {
            ps_st.printf(PRINTF_NG_NULL_ARG);
        }
         break;

//      GPS Off/GPS on on Transmit only/On-------------------------------------
    case 'G':
        #if HAS_GPS == 1
            if (command[0] != '\0') {
                #define PRINTF_OK_GPS "OK:GPS = %u (%s)\r\n"
                #define PRINTF_NG_GPS "NG:GPS = %u must be 0 for OFF, 1 for ON AT TX or 2 for ON\r\n"
    
    
                local_params.gps_state = PARMS.parameters.gps_state;
                if (parameter_query) {
                    ps_st.printf(PRINTF_OK_GPS, PARMS.parameters.gps_state, GPS.getPowerStateName((GPSClass::PowerState) PARMS.parameters.gps_state));
                }
                else {
                    local_params.gps_state = atoi(command);
                    if ((int_input >= 0 && int_input <= 2) && is_numeric(command)) {
                        PARMS.parameters.gps_state = local_params.gps_state;
                        ps_st.printf(PRINTF_OK_GPS, (int)PARMS.parameters.gps_state, GPS.getPowerStateName((GPSClass::PowerState) local_params.gps_state));
                    }
                    else {
                        ps_st.printf(PRINTF_NG_GPS, local_params.gps_state);
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
    //case '?':
        ps_st.printf("WiFi credentials                 /@<ssid>,<passcode>   Note: case\r\n");
        ps_st.printf("                                   sensitive and spaces not permitted!\r\n");
        ps_st.printf("Radio Address                    /A <n>\r\n");
        ps_st.printf("Beacon Disable (TX Lockout)      /B <off>|<on>\r\n");
        ps_st.printf("Caallsign                        /C <callsign>\r\n");
        ps_st.printf("Reset radio to default state     /D\r\n");        
        ps_st.printf("Frequency                        /F <Frequency in MHz>\r\n");
        ps_st.printf("GPS State                        /G 0 for OFF, 1 for ON FOR TX\r\n");
        ps_st.printf("                                    and 2 for ON<off>|<tx<on>\r\n");
        ps_st.printf("Help Text                        /H\r\n");
        ps_st.printf("TX Interval (seconds)            /I <n>\r\n");
        ps_st.printf("Position                         /L <latitude >,<longitude>\r\n");
        ps_st.printf("Modulation index 0<=n<=8         /M <n>\r\n");
        ps_st.printf("Power index 0<=n<=6              /P <n>\r\n");
        ps_st.printf("RF Signal Reports (CSV)          /R 0=Off, 1=Serial, 2=TELNET, 3=Both\r\n");
        ps_st.printf("Radio Type                       /T <n>\r\n");
        ps_st.printf("Promiscuous Mode                 /U <off>|<on>\r\n");
        ps_st.printf("                                    Note: Special compile required\r\n");
        ps_st.printf("Version number                   /V\r\n");
        ps_st.printf("Write to NVRAM                   /W\r\n");
        ps_st.printf("Maidenhead grid square (4 or 6)  /X\r\n");
        ps_st.printf("Short TX Pause                   /Y <off>|<on>\r\n");
        ps_st.printf("Commands case insensitive and blanks ignored\r\n");
        break;

//      Interval (transmit)----------------------------------------------------

    case 'I':
        local_params.tx_interval = PARMS.parameters.tx_interval;
        cli_process_int(parameter_query, PARMS.Key.tx_interval, command, 10, 255 , & local_params.tx_interval);
        PARMS.parameters.tx_interval = local_params.tx_interval;
        break;

//      Location---------------------------------------------------------------
    case 'L':
        if (parameter_query) {
            ps_st.printf("OK:Latitude = % f; Longitude = %f\r\n", PARMS.parameters.lat_value, PARMS.parameters.lon_value);
        }
        else {
            comma_found = false;
            i = 0;
            lat_i = 0;
            lon_i = 0;
            lat_str[0] = '\0';
            lon_str[0] = '\0';
            lat_sign = 1;
            lon_sign = 1;
            while (command[i] != '\0') {
                if (command[i] != ',') {
                    if (!comma_found) {
                        if (lat_i < sizeof(lat_str)-1) {
                            lat_str[lat_i] = command[i];
                            lat_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal latitude buffer exceeded\r\n");
                            return 1;
                        }
                    }
                    else {
                        if (lon_i < sizeof(lon_str)-1) {
                            lon_str[lon_i] = command[i];
                            lon_i++;
                        }
                        else {
                            ps_st.printf("NG:Internal longitude buffer exceeded\r\n");
                            return 1;
                        }
                    }
                }
                else {
                        comma_found = true;
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
    
            if (!comma_found) {
                ps_st.printf("NG:Location requires comma separation between Lat & Lon\r\n");
                return 1;
            }
            if (!(is_float(lon_str_ptr) && is_float(lat_str_ptr))) {
                ps_st.printf("NG:Latitude and Longitude must be valid floating point numbers\r\n");
                return 1;
            }
            local_params.lat_value = atof(lat_str_ptr) * lat_sign;
            local_params.lon_value = atof(lon_str_ptr) * lon_sign;
    
            if (abs(local_params.lat_value) > 90) {
                ps_st.printf("NG:Longitude must be between -90 and 90\r\n");
                return 1;
            }
            if (abs(local_params.lon_value) > 180) {
                ps_st.printf("NG:Longitude must be between -180 and 180\r\n");
                return 1;
            }
            PARMS.parameters.lat_value = local_params.lat_value;
            PARMS.parameters.lon_value = local_params.lon_value;    
            ps_st.printf("OK:Latitude = % f; Longitude = %f\r\n", PARMS.parameters.lat_value, PARMS.parameters.lon_value);
        }

        break;

//      Modulation-------------------------------------------------------------
        case 'M':
            if (command[0] != '\0') {
                local_params.modulation_index  = PARMS.parameters.modulation_index;
                cli_process_index_char_value_unit(parameter_query, "Modulation", command, 0, sizeof(modulation_array)/sizeof(modulation_array[0])-1, modulation_array,  & local_params.modulation_index);
                PARMS.parameters.modulation_index = local_params.modulation_index;
                setModemConfig(local_params.modulation_index); //SF Bandwith etc
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
            break;

//      Power------------------------------------------------------------------
        case 'P':
            if (command[0] != '\0') {
                local_params.power_index = PARMS.parameters.power_index;
                cli_process_index_float_value_unit(parameter_query, "Power", command, 0, sizeof(power)/sizeof(power[0])-1, power , "dBm",  & local_params.power_index);
                PARMS.parameters.power_index = local_params.power_index;
                driver.setTxPower(power[local_params.power_index]);
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
            }
            break;

//  Quit Telnet session----------------------------------------
        case 'Q':
            ps_st.printf("OK:Quitting Telnet session\r\n");
            break;

//      CSV Output Off/On------------------------------------------------------
        case 'R':
            csv_current_state = 0;
            if (PARMS.parameters.serialCSVEnabled) csv_current_state = csv_current_state += 1;
            if (PARMS.parameters.telnetCSVEnabled) csv_current_state = csv_current_state += 2;
            cli_process_index_char_value_unit(parameter_query, "CSV Output", command, 0, sizeof(csv_array)/sizeof(csv_array[0])-1, csv_array,  & csv_current_state);
            PARMS.parameters.serialCSVEnabled = (csv_current_state % 2) == 1;
            PARMS.parameters.telnetCSVEnabled = int (csv_current_state / 2) == 1;
            csv_serial.setOutputEnabled(PARMS.parameters.serialCSVEnabled);
            csv_telnet.setOutputEnabled(PARMS.parameters.telnetCSVEnabled);
            break;

//      Radio Type-------------------------------------------------------------
        case 'T':
            if (command[0] != '\0')
            {
                local_params.radioType = PARMS.parameters.radioType;
                cli_process_int(parameter_query, "Radio Type", command, 0, 3 , & local_params.radioType);
                PARMS.parameters.radioType = local_params.radioType;
            }
            else {
                ps_st.printf(PRINTF_NG_NULL_ARG);
             }
            break;

//      Promiscuous Mode
        case 'U':
            local_params.promiscuousEnabled      = PARMS.parameters.promiscuousEnabled;
            cli_process_bool(parameter_query, "Promiscuous Mode", command, & local_params.promiscuousEnabled);
            PARMS.parameters.promiscuousEnabled = local_params.promiscuousEnabled;
            driver.setPromiscuous(local_params.promiscuousEnabled);
            break;

//      Version---------------------------------------------------------
        case 'V':
            ps_st.printf("OK: Version: %s\r\nOK: Build date: %s %s\r\n", "TBD", VERSION_DATE, VERSION_TIME); 
            break;

//      Write to NVRAM---------------------------------------------------------
        case 'W':
            PARMS.update(); // update current settings in parameters structure to nvs
            ps_st.printf("OK: Non-volatile storage updated with current values\r\n"); 
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
                    decode_grid4(PARMS.parameters.grid4, current_str_value);
                   ps_st.printf("OK:Grid4=%s (%u). Grid5=%c, Grid6=%c\r\n", current_str_value, (unsigned int)PARMS.parameters.grid4, PARMS.parameters.grid5, PARMS.parameters.grid6);
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
            local_params.short_pause = PARMS.parameters.short_pause;    
            cli_process_bool(parameter_query, "Short Pause State", command, & local_params.short_pause);
            PARMS.parameters.short_pause = local_params.short_pause;
            break;

//      Invalid Command--------------------------------------------------------
        default:
            ps_st.printf("NG:Unrecognized command %c [@, A, B, C, F, G, H, I, L, M, P, R, T, U, V, W, X, Y]\r\n", cmd_code);
        }
    }
    else {
        ps_st.printf("NG:Leading forward slash missing\r\n");

        return 1;
    }

    return 0;
}

//=============================================================================
int cli_execute(const char* command_arg) {
    //int cli_show_all(const char* command_arg) {
        int i;
    
        char command[50],command_original_case[50];
        char command_query[4];
        char command_codes[]={'@','A', 'B', 'C', 'F', 'G', 'I', 'L', 'M', 'P', 'R', 'T', 'U', 'V', 'X', 'Y'};
    
        strcpy(command, command_arg);
    
        if ((command[0] == '/') && command[1] == '?') {
    
            for (i=0; i < sizeof(command_codes)/sizeof(command_codes[0]); ++i) 
            {
                command[0] = '/';
                command[1] = command_codes[i];
                command[2] = '?';
                command[3] = '\0';
    
                cli_execute_command(command);
            }
        }
        else {
            return cli_execute_command(command_arg);
        }
    
        return 0;
    }
    
    
