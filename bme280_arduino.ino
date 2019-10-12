// I N C L U D E S
//-------------------------------------------------------------------------------------------------
#include <Wire.h>
#include "bme280.h"

// D E F I N E S
//-------------------------------------------------------------------------------------------------
#define BAUD_RATE 115200
#define dev_addr 0x76

// G L O B A L  V A R I A B L E S
//-------------------------------------------------------------------------------------------------
float pressure;
float temperature;
float humidity;

uint8_t chip_id;

char payload[32];

BME280 sensor_bme280;

// S E T U P
//-------------------------------------------------------------------------------------------------
void setup()
{

  Wire.begin();
  Serial.begin(BAUD_RATE);

  Serial.println("BME280 - Temperature, Humidity, and Pressure Sensor\n");
  Serial.print("Chip ID = 0x");
  Serial.print(sensor_bme280.begin_i2c(), HEX);
  Serial.println("\n");

  sensor_bme280.set_humid_osrs(bme280_osrs_two);
  sensor_bme280.set_temp_osrs(bme280_osrs_two);
  sensor_bme280.set_pres_osrs(bme280_osrs_two);
  sensor_bme280.set_mode(bme280_forced_mode);

  Serial.println("Settings");
  Serial.println("----------------------------------------------\r");
  Serial.print("CTRL HUM\t0x");
  Serial.print(sensor_bme280.read_reg(bme280_ctrl_hum_addr), HEX);
  Serial.print("\nCTRL MEAS\t0x");
  Serial.print(sensor_bme280.read_reg(bme280_ctrl_meas_addr), HEX);

  temperature = sensor_bme280.read_float_temp_f();
  pressure = sensor_bme280.read_float_pres();
  humidity = sensor_bme280.read_float_humidity();

  sprintf(payload, "%d\t%f\t%f", temperature, pressure, humidity);
  Serial.println("\n\nTemp\t\tHumidity\tPressure");
  Serial.println("----------------------------------------------\r");
  Serial.print(temperature);
  Serial.print(" F \t");
  Serial.print(humidity);
  Serial.print(" % \t");
  Serial.print(pressure);
  Serial.print(" hPa \t\n");
}

void getData()
{
  temperature = sensor_bme280.read_float_temp_f();
  pressure = sensor_bme280.read_float_pres();
  humidity = sensor_bme280.read_float_humidity();
  Serial.print(temperature);
  Serial.print(" F \t");
  Serial.print(humidity);
  Serial.print(" % \t");
  Serial.print(pressure);
  Serial.print(" hPa \t\n");
}

// L O O P
//-------------------------------------------------------------------------------------------------
void loop()
{
  delay(10e3);
  getData();
}
