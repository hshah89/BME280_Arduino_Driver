/*  These drivers are for the arduino IDE platform. 
    It also can be used with ESP8266 and ESP32.
    Really, any devices that can be connected using the I2C library
    that comes with the Arduino IDE

    This library for now only works with I2C wire interface. 
    
    Support for Two and Three wire SPI is coming at a later point.

    Author: Harit Shah

    References: BME280 Datasheet, BME280 C drivers
*/

#include "Arduino.h"
#include <Wire.h>

// Defines

/* Chip ID */
#define bme280_chip_id              0x60

/* Temperature Calibration Address */
#define bme280_dig_T1_lsb_reg       0x88
#define bme280_dig_T1_msb_reg       0x89
#define bme280_dig_T2_lsb_reg       0x8A
#define bme280_dig_T2_msb_reg       0x8B
#define bme280_dig_T3_lsb_reg       0x8C
#define bme280_dig_T3_msb_reg       0x8D

/* Pressure Calibration Address */
#define bme280_dig_P1_lsb_reg       0x8E
#define bme280_dig_P1_msb_reg       0x8F
#define bme280_dig_P2_lsb_reg       0x90
#define bme280_dig_P2_msb_reg       0x91
#define bme280_dig_P3_lsb_reg       0x92
#define bme280_dig_P3_msb_reg       0x93
#define bme280_dig_P4_lsb_reg       0x94
#define bme280_dig_P4_msb_reg       0x95
#define bme280_dig_P5_lsb_reg       0x96
#define bme280_dig_P5_msb_reg       0x97
#define bme280_dig_P6_lsb_reg       0x98
#define bme280_dig_P6_msb_reg       0x99
#define bme280_dig_P7_lsb_reg       0x9A
#define bme280_dig_P7_msb_reg       0x9B
#define bme280_dig_P8_lsb_reg       0x9C
#define bme280_dig_P8_msb_reg       0x9D
#define bme280_dig_P9_lsb_reg       0x9E
#define bme280_dig_P9_msb_reg       0x9F

/* Humidity Calibration Address */
#define bme280_dig_H1_lsb_reg       0xA1
#define bme280_dig_H2_lsb_reg       0xE1
#define bme280_dig_H2_msb_reg       0xE2
#define bme280_dig_H3_lsb_reg       0xE3
#define bme280_dig_H4_msb_reg       0xE4
#define bme280_dig_H4_lsb_reg       0xE5
#define bme280_dig_H5_lsb_reg       0xE5
#define bme280_dig_H5_msb_reg       0xE6
#define bme280_dig_H6_lsb_reg       0xE7

/* Register Address */  
#define bme280_chip_id_addr         0xD0
#define bme280_reset_addr           0xE0
#define bme280_ctrl_hum_addr        0xF2
#define bme280_status_addr          0xF3
#define bme280_ctrl_meas_addr       0xF4
#define bme280_config_addr          0xF5
#define bme280_press_msb_addr       0xF7
#define bme280_press_lsb_addr       0xF8
#define bme280_press_xlsb_addr      0xF9
#define bme280_temp_msb_addr        0xFA
#define bme280_temp_lsb_addr        0xFB
#define bme280_temp_xlsb_addr       0xFC
#define bme280_hum_msb_addr         0xFD
#define bme280_hum_lsb_addr         0xFE

/* Sensor Power Mode */
#define bme280_sleep_mode           0b00
#define bme280_forced_mode          0b01
#define bme280_normal_mode          0b11

/* Oversampling Masks */
#define bme280_osrs_skipped         0x00
#define bme280_osrs_one             0x01
#define bme280_osrs_two             0x02
#define bme280_osrs_four            0x03
#define bme280_osrs_eight           0x04
#define bem280_osrs_sixteen         0x05

/* standby time ms */
#define bme280_t_sb_0_5             0x00
#define bme280_t_sb_62_5            0x01
#define bme280_t_sb_125_0           0x02
#define bme280_t_sb_250_0           0x03
#define bme280_t_sb_500_0           0x04
#define bme280_t_sb_1000_0          0x05
#define bme280_t_sb_10_0            0x06
#define bme280_t_sb_20_0            0x07

/* Filter settings */
#define bme280_filter_off           0x00
#define bme280_filter_two           0x01
#define bme280_filter_four          0x02
#define bme280_filter_eight         0x03
#define bem280_filter_sixteen       0x04

/* Calibration Data */
struct bme280_calib_data 
{
    public:
        uint16_t  dig_T1;
        int16_t   dig_T2;
        int16_t   dig_T3;

        uint16_t  dig_P1;
        int16_t   dig_P2;
        int16_t   dig_P3;
        int16_t   dig_P4;
        int16_t   dig_P5;
        int16_t   dig_P6;
        int16_t   dig_P7;
        int16_t   dig_P8;
        int16_t   dig_P9;

        uint8_t   dig_H1;
        int16_t   dig_H2;
        uint8_t   dig_H3;
        int16_t   dig_H4;
        int16_t   dig_H5;
        int8_t    dig_H6;

       

        uint8_t temp_reg_data[6];
        uint8_t pres_reg_data[18];
        uint8_t humid_reg_data_a;
        uint8_t humid_reg_data_b[7];
};

struct bme280_sensor_settings
{
    public:
        uint8_t i2c_addr;
        float temp_correction;

};

class BME280
{
    public:

        bme280_calib_data calibration;
        bme280_sensor_settings settings;

        int32_t t_fine;

        BME280(void);

        void begin(void);
        uint8_t begin_i2c(TwoWire &wirePort = Wire);

        uint8_t get_chip_id(void);
        void get_calib_data(void);

        uint8_t get_mode(void);
        void set_mode(uint8_t mode);

        void set_temp_osrs(uint8_t osrs_value);
        void set_pres_osrs(uint8_t osrs_value);
        void set_humid_osrs(uint8_t osrs_value);
        void set_standby_time(uint8_t time_setting);
        void set_filter(uint8_t filter_setting);

        void reset(void);

        float read_float_pres(void);
        float read_float_humidity(void);
        float read_float_temp_c(void);
        float read_float_temp_f(void);

        uint8_t read_reg(uint8_t offset);
        void read_reg_range(uint8_t *output, uint8_t offset, uint8_t length);
        void write_reg(uint8_t offset, uint8_t write_data);
        uint16_t concat_bytes(uint8_t msb, uint8_t lsb);

};

