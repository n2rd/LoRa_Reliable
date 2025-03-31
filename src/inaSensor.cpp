#include "main.h"

#include "inaSensor.h"

INASensor INA219;

void INASensor::setup() 
{ 
  // Initialize the INA sensor 
  if (!ina219.begin(&Wire1)) { // can use other I2C addresses or buses
    log_d("Failed to find INA sensor chip");
  } else {
    bIsPresent = true;
    log_v("INA Sensor found");
  
    // Set the INA219 to use the 32V, 1A range instead of default 2A lower precision
    //ina219.setCalibration_32V_2A();
    // Set the INA219 to use the 16V, 400mA range instead of default 2A lower precision
    //ina219.setCalibration_16V_400mA();
    // Set the INA219 to use the 32V, 1A range instead of default 2A lower precision
    ina219.setCalibration_32V_1A();
  }
} //::setup

void INASensor::outputData(Print& printDev) {
  // Display voltage and current (in mA) just for channel 0 
  float voltage = getBusVoltage_V();
  float current = getCurrent_mA(); 
  printDev.printf("Vol %4.2fV  Cur %3.0fmA\n", voltage, current);
}

bool INASensor::isPresent() {
  return bIsPresent;
}

float INASensor::getBusVoltage_V()
{
  return ina219.getBusVoltage_V();
}

float INASensor::getCurrent_mA()
{
  return ina219.getCurrent_mA();
}