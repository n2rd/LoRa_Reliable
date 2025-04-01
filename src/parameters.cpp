#include "Preferences.h"
#include "myConfig.h"
#include "parameters.h"

ParametersClass PARMS;

Preferences ParametersClass::preferences;
//
//@brief Dump a key value thats a Uint type
//
void ParametersClass::DumpKeyUint(char *keyToDump) {
      if (preferences.isKey(keyToDump)) {
            log_e("Address key exists");
            uint8_t temp = preferences.getUInt(keyToDump);
            log_e("Address: %u",temp);
      }
      else log_e("No key: %s",keyToDump);
}
//
//@brief Dump a key value thats a string
//
void ParametersClass::DumpKeyString(char *keyToDump) {
      if (preferences.isKey(keyToDump)) {
            log_e("Callsign key exists");
            String stringValue = preferences.getString(keyToDump);
            char* temp = (char *) stringValue.c_str();
            int len = strlen(temp);
            log_e("callsign:(%i), %s",len,temp);
      }
      else log_e("No key: %s",keyToDump);
}
//
//@brief parameter initialization from nv ram if it exits else use defaults
//
void ParametersClass::init() {
  //each parameter is tested individually to see if it exists in the preferences 
  //  so if there are changes in the parameters they will be picked up
  // if the preferences do not exist, the default is written to the preferences nvram
  if (!preferences.begin("LoRa", RW_MODE)) {  //R/W needed to create it if it does not exist
      //failure to open .... pass Partion value to initialize the partion
      log_e("Preferences.begin () Failed to begin paramters group Lora. Trying to initialize.");
      if (!preferences.begin("LoRa", RW_MODE, "nvs") /*||true*/) {
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
            parameters.serialCSVEnabled = true;
            parameters.telnetCSVEnabled = true;
            strcpy(parameters.wifiSSID,DEFAULT_WIFI_SSID);
            strcpy(parameters.wifiKey,DEFAULT_WIFI_KEY);
            parameters.p2pAddressFilterEnabled = true;
            parameters.promiscuousEnabled = DEFAULT_PROMISCUOUS;
            parameters.gridSize = DEFAULT_GRID_SIZE;
            strcpy(parameters.csvFilter,DEFAULT_CSV_FILTER);
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
  if (preferences.isKey(Key.modulation_index)) {
        // Preferences exist, read from it and put into mypreferences
        parameters.modulation_index = preferences.getUInt(Key.modulation_index);
  } else { //use defaults and write to nvram
        parameters.modulation_index = DEFAULT_MODULATION_INDEX;
        preferences.putUInt(Key.modulation_index, parameters.modulation_index);
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
  if (preferences.isKey(Key.wifiSSID)) {
      strcpy(parameters.wifiSSID, preferences.getString(Key.wifiSSID).c_str());
  } else { //use defaults and write to nvram
      strcpy(parameters.wifiSSID, DEFAULT_WIFI_SSID);
      preferences.putString(Key.wifiSSID, parameters.wifiSSID);
  }
  if (preferences.isKey(Key.wifiKey)) {
      strcpy(parameters.wifiKey, preferences.getString(Key.wifiKey).c_str());
  } else { //use defaults and write to nvram
      strcpy(parameters.wifiKey, DEFAULT_WIFI_KEY);
      preferences.putString(Key.wifiKey, parameters.wifiKey);
  }        
  if (preferences.isKey(Key.serialCSVEnabled)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.serialCSVEnabled = preferences.getUInt(Key.serialCSVEnabled);
} else { //use defaults and write to nvram
      parameters.serialCSVEnabled = true;
      preferences.putUInt(Key.serialCSVEnabled, parameters.serialCSVEnabled);
}
if (preferences.isKey(Key.telnetCSVEnabled)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.telnetCSVEnabled = preferences.getUInt(Key.telnetCSVEnabled);
} else { //use defaults and write to nvram
      parameters.telnetCSVEnabled = true;
      preferences.putUInt(Key.telnetCSVEnabled, parameters.telnetCSVEnabled);
}
if (preferences.isKey(Key.promiscuousEnabled)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.promiscuousEnabled = preferences.getUInt(Key.promiscuousEnabled);
} else { //use defaults and write to nvram
      parameters.promiscuousEnabled = DEFAULT_PROMISCUOUS;
      preferences.putUInt(Key.promiscuousEnabled, parameters.promiscuousEnabled);
}
if (preferences.isKey(Key.p2pAddressFilterEnabaled)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.p2pAddressFilterEnabled = preferences.getUInt(Key.p2pAddressFilterEnabaled);
} else { //use defaults and write to nvram
      parameters.p2pAddressFilterEnabled = DEFAULT_P2P_ADDRESS_FILTERING;
      preferences.putUInt(Key.p2pAddressFilterEnabaled, parameters.p2pAddressFilterEnabled);
}
if (preferences.isKey(Key.radioType)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.radioType = preferences.getUInt(Key.radioType);
} else { //use defaults and write to nvram
      parameters.radioType = 0;
      preferences.putUInt(Key.radioType, parameters.radioType);
}
if (preferences.isKey(Key.gridSize)) {
      // Preferences exist, read from it and put into mypreferences
      parameters.gridSize = preferences.getUInt(Key.gridSize);
} else { //use defaults and write to nvram
      parameters.gridSize = DEFAULT_GRID_SIZE;
      preferences.putUInt(Key.gridSize, parameters.gridSize);
}
if (preferences.isKey(Key.csvFilter)) {
      // Preferences exist, read from it and put into mypreferences
      strcpy(parameters.csvFilter, preferences.getString(Key.csvFilter).c_str());
} else { //use defaults and write to nvram
      strcpy(parameters.csvFilter, DEFAULT_CSV_FILTER);
      preferences.putString(Key.csvFilter, parameters.csvFilter);
}

preferences.end();
}
//
//@brief update nvs from parameters if they have changed
//
void ParametersClass::update() {
    //log_e("start parameter update");
    if (!preferences.begin("LoRa", RW_MODE)) {
      log_e("Couldn't open preferences RW");
    }
    //update the parameters in the preferences
    //write only new values as writing to the preferences is slow
    if (strcmp(preferences.getString(Key.callsign).c_str(), parameters.callsign) != 0) {
        preferences.putString(Key.callsign, parameters.callsign);
    }
    if (preferences.getUInt(Key.frequency_index) != parameters.frequency_index) {
        preferences.putUInt(Key.frequency_index, parameters.frequency_index);
    }
    if (preferences.getUInt(Key.gps_state) != parameters.gps_state) {
        preferences.putUInt(Key.gps_state, parameters.gps_state);
    }
    if (preferences.getUInt(Key.tx_lock) != parameters.tx_lock) {
        preferences.putUInt(Key.tx_lock, parameters.tx_lock);
    }
    if (preferences.getUInt(Key.short_pause) != parameters.short_pause) {
        preferences.putUInt(Key.short_pause, parameters.short_pause);
    }
    if (preferences.getFloat(Key.lat_value) != parameters.lat_value) {
        preferences.putFloat(Key.lat_value, parameters.lat_value);
    }
    if (preferences.getFloat(Key.lon_value) != parameters.lon_value) {
        preferences.putFloat(Key.lon_value, parameters.lon_value);
    }
    if (preferences.getUInt(Key.grid4) != parameters.grid4) {
        preferences.putUInt(Key.grid4, parameters.grid4);
    }
    if (preferences.getChar(Key.grid5) != parameters.grid5) {
        preferences.putChar(Key.grid5, parameters.grid5);
    }
    if (preferences.getChar(Key.grid6) != parameters.grid6) {
        preferences.putChar(Key.grid6, parameters.grid6);
    }
    if (preferences.getUInt(Key.modulation_index) != parameters.modulation_index) {
        preferences.putUInt(Key.modulation_index, parameters.modulation_index);
    }
    if (preferences.getUInt(Key.power_index) != parameters.power_index) {
        preferences.putUInt(Key.power_index, parameters.power_index);
    }
    if (preferences.getUInt(Key.tx_interval) != parameters.tx_interval) {
        preferences.putUInt(Key.tx_interval, parameters.tx_interval);
    }
    if (preferences.getUInt(Key.address) != parameters.address) {
        preferences.putUInt(Key.address, parameters.address);
    }
    if (preferences.getUInt(Key.radioType) != parameters.radioType) {
      preferences.putUInt(Key.radioType, parameters.radioType);
    }
    if (strcmp(preferences.getString(Key.wifiSSID).c_str(), parameters.wifiSSID) != 0) {
      preferences.putString(Key.wifiSSID, parameters.wifiSSID);
    }
    if (strcmp(preferences.getString(Key.wifiKey).c_str(), parameters.wifiKey) != 0) {
      preferences.putString(Key.wifiKey, parameters.wifiKey);
    }
    if (preferences.getUInt(Key.serialCSVEnabled) != parameters.serialCSVEnabled) {
      preferences.putUInt(Key.serialCSVEnabled, parameters.serialCSVEnabled);
    }
    if (preferences.getUInt(Key.telnetCSVEnabled) != parameters.telnetCSVEnabled) {
      preferences.putUInt(Key.telnetCSVEnabled, parameters.telnetCSVEnabled);
    }
    if (preferences.getUInt(Key.promiscuousEnabled) != parameters.promiscuousEnabled) {
      preferences.putUInt(Key.promiscuousEnabled, parameters.promiscuousEnabled);
    }
    if (preferences.getUInt(Key.p2pAddressFilterEnabaled) != parameters.p2pAddressFilterEnabled) {
      preferences.putUInt(Key.p2pAddressFilterEnabaled, parameters.p2pAddressFilterEnabled);
    }
    if (preferences.getUInt(Key.gridSize) != parameters.gridSize) {
      preferences.putUInt(Key.gridSize, parameters.gridSize);
    }
    if (preferences.getUInt(Key.gridSize) != parameters.gridSize) {
      preferences.putUInt(Key.gridSize, parameters.gridSize);
    }
    if (strcmp(preferences.getString(Key.csvFilter).c_str(), parameters.csvFilter) !=0) {
      preferences.putString(Key.csvFilter, parameters.csvFilter);
    }

    preferences.end();
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

//activate parameters based on values in parameters struct
bool ParametersClass::set_frequency() {
  return driver.setFrequency(PARMS.frequency_index_to_frequency(PARMS.parameters.frequency_index));
} 

bool ParametersClass::set_power() {
  driver.setTxPower(power[PARMS.parameters.power_index]);
  return true;
}

bool ParametersClass::set_modulation() {
  return setModemConfig(PARMS.parameters.modulation_index); //SF Bandwith etc contained in radio specific code
}

void ParametersClass::set_address() {
  manager.setThisAddress(PARMS.parameters.address);
}