#include "myConfig.h"
#ifdef HAS_INA219
#include "main.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

#include "inaSensor.h"

// Create an INA3221 object
Adafruit_INA219 ina219;

void ina_setup() 
{ 
  //Wire1.begin(41U,42U,400000);
  Wire1.begin(41U,42U);
  // Initialize the INA sensor 
  if (!ina219.begin(&Wire1)) { // can use other I2C addresses or buses
    Serial.println("Failed to find INA sensor chip");
  } else {
    Serial.println("INA Sensor found");
  }
  // Set the INA219 to use the 32V, 1A range instead of default 2A lower precision
  //ina219.setCalibration_32V_2A();
  // Set the INA219 to use the 16V, 400mA range instead of default 2A lower precision
  //ina219.setCalibration_16V_400mA();
  // Set the INA219 to use the 32V, 1A range instead of default 2A lower precision
  ina219.setCalibration_32V_1A();
} //ina_setup

void ina_measure() {
  // Display voltage and current (in mA) just for channel 0 
 float voltage = ina219.getBusVoltage_V();
 float current = ina219.getCurrent_mA(); 
 Serial.printf("Vol %4.2fV  Cur %3.0fmA\n", voltage, current);
 ps_all.printf("Vol %4.2fV  Cur %3.0fmA\n", voltage, current);
}


#endif //HAS_INA3221