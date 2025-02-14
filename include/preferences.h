#ifndef PREFERENCES_H
#define PREFERENCES_H

typedef struct pref {
    char callsign[10];
    float frequency;
    bool gps_state;
    float lat_value, lon_value;
    int modulation_index;
    int power_index;
    int tx_interval;
    int radio_id;
} PREFERENCES;

extern PREFERENCES preferences;

#endif //PREFERENCES_H