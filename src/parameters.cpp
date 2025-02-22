#include "Preferences.h"
#include "myConfig.h"
#include "parameters.h"

ParametersClass PARMS;
//
//@brief parameter initialization from nv ram if it exits else use defaults
//
void ParametersClass::init(){
  //each parameter is tested individually to see if it exists in the preferences 
  //  so if there are changes in the parameters they will be picked up
  // if the preferences do not exist, the default is written to the preferences nvram
  if (!preferences.begin("LoRa", RW_MODE)) {  //R/W needed to create it if it does not exist
      //failure to open .... pass Partion value to initialize the partion
      log_e("Preferences.begin () Failed to begin paramters group Lora. Trying to initialize.");
      if (!preferences.begin("LoRa", RW_MODE, "nvs")) {
            //Failed again
            log_e("nvs_flash_init_partition failed. Using defaults");
            //use defaults here
            strcpy(parameters.callsign, DEFAULT_CALLSIGN);
            parameters.frequency_index = DEFAULT_FREQUENCY_INDEX;
            parameters.gps_state = DEFAULT_GPS_STATE;
            parameters.tx_lock = DEFAULT_TX_LOCK;
            parameters.short_pause = DEFAULT_SHORT_PAUSE;
            parameters.lat_value = DEFAULT_LAT_VALUE;
            parameters.lon_value = DEFAULT_LON_VALUE;
            parameters.grid4 = DEFAULT_GRID4;
            parameters.grid5 = DEFAULT_GRID5;
            parameters.grid6 = DEFAULT_GRID6;
            parameters.modulation_index = DEFAULT_MODULATION_INDEX;
            parameters.power_index = DEFAULT_POWER_INDEX;
            parameters.tx_interval = DEFAULT_TX_INTERVAL;
            parameters.address = DEFAULT_ADDRESS;
            return;
      }
      log_e("nvs_flash_init_partition Succeeded");
  }
  if (preferences.isKey(Key.callsign)) {
        strcpy(parameters.callsign, preferences.getString(Key.callsign).c_str());
  } else { //use defaults and write to nvram
        strcpy(parameters.callsign, DEFAULT_CALLSIGN);
        preferences.putString(Key.callsign, parameters.callsign);
  }
  if (preferences.isKey(Key.frequency_index)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.frequency_index = preferences.getUInt(Key.frequency_index);
  } else { //use defaults and write to nvram
        parameters.frequency_index = DEFAULT_FREQUENCY_INDEX;
        preferences.putUInt(Key.frequency_index, parameters.frequency_index);
  }
  if (preferences.isKey(Key.gps_state)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.gps_state = preferences.getUInt(Key.gps_state);
  } else { //use defaults and write to nvram
        parameters.gps_state = DEFAULT_GPS_STATE;
        preferences.putUInt(Key.gps_state, parameters.gps_state);
  }
  if (preferences.isKey(Key.tx_lock)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.tx_lock = preferences.getUInt(Key.tx_lock);
  } else { //use defaults and write to nvram
        parameters.tx_lock = DEFAULT_TX_LOCK;
        preferences.putUInt(Key.tx_lock, parameters.tx_lock);
  } 
  if (preferences.isKey(Key.short_pause)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.short_pause = preferences.getUInt(Key.short_pause);
  } else { //use defaults and write to nvram
        parameters.short_pause = DEFAULT_SHORT_PAUSE;
        preferences.putUInt(Key.short_pause, parameters.short_pause);
  } 
  if (preferences.isKey(Key.lat_value)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.lat_value = preferences.getFloat(Key.lat_value);
  } else { //use defaults and write to nvram
        parameters.lat_value = DEFAULT_LAT_VALUE;
        preferences.putFloat(Key.lat_value, parameters.lat_value);
  }
  if (preferences.isKey(Key.lon_value)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.lon_value = preferences.getFloat(Key.lon_value);
  } else { //use defaults and write to nvram
        parameters.lon_value = DEFAULT_LON_VALUE;
        preferences.putFloat(Key.lon_value, parameters.lon_value);
  }
  if (preferences.isKey(Key.grid4)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.grid4 = preferences.getUInt(Key.grid4);
  } else { //use defaults and write to nvram
        parameters.grid4 = DEFAULT_GRID4;
        preferences.putUInt(Key.grid4, parameters.grid4);
  }
  if (preferences.isKey(Key.grid5)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.grid5 = preferences.getChar(Key.grid5);
  } else { //use defaults and write to nvram
        parameters.grid5 = DEFAULT_GRID5;
        preferences.putChar(Key.grid5, parameters.grid5);
  }
  if (preferences.isKey(Key.grid6)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.grid6 = preferences.getChar(Key.grid6);
  } else { //use defaults and write to nvram
        parameters.grid6 = DEFAULT_GRID6;
        preferences.putChar(Key.grid6, parameters.grid6);
  }
  if (preferences.isKey(Key.modultation_index)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.modulation_index = preferences.getUInt(Key.modultation_index);
  } else { //use defaults and write to nvram
        parameters.modulation_index = DEFAULT_MODULATION_INDEX;
        preferences.putUInt(Key.modultation_index, parameters.modulation_index);
  }
  if (preferences.isKey(Key.power_index)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.power_index = preferences.getUInt(Key.power_index);
  } else { //use defaults and write to nvram
        parameters.power_index = DEFAULT_POWER_INDEX;
        preferences.putUInt(Key.power_index, parameters.power_index);
  }
  if (preferences.isKey(Key.tx_interval)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.tx_interval = preferences.getUInt(Key.tx_interval);
  } else { //use defaults and write to nvram
        parameters.tx_interval = DEFAULT_TX_INTERVAL;
        preferences.putUInt(Key.tx_interval, parameters.tx_interval);
  }
  if (preferences.isKey(Key.address)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.address = preferences.getUInt(Key.address);
  } else { //use defaults and write to nvram
        parameters.address = DEFAULT_ADDRESS;
        preferences.putUInt(Key.address, parameters.address);
  }
}

//
//now for some bridge functions that turn indexes into useful values
//

//set the center frequency in MHz from the index
float ParametersClass::frequency_index_to_frequency(uint8_t index) {
  return 902.125 + 0.25 * index;
}

//get the center frequency in MHz from the index
uint8_t ParametersClass::frequency_to_frequency_index(float frequency) {
  return (frequency - 902.125) / 0.25;
}

size_t ParametersClass::putString(const char *key, char *value) {
      return preferences.putString(key,value);
}
size_t ParametersClass::putUInt8(const char *key, uint8_t byte ) {
      return preferences.putUInt(key, byte);
}
size_t ParametersClass::putUInt16(const char *key, uint16_t word ) {
      return preferences.putUInt(key, word);
}
size_t ParametersClass::putChar(const char *key, char value) {
      return preferences.putChar(key, value);
}
size_t ParametersClass::putFloat(const char *key, float value) {
      return preferences.putFloat(key, value);
}

