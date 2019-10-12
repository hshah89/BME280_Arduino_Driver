/*  These drivers are for the arduino IDE platform. 
    It also can be used with ESP8266 and ESP32.
    Really, any devices that can be connected using the I2C library
    that comes with the Arduino IDE

    This library for now only works with I2C wire interface. 
    
    Support for Two and Three wire SPI is coming at a later point.

    Author: Harit Shah

    References: BME280 Datasheet, BME280 C drivers
*/

#include "bme280.h";

/* Settings, Configuration */

BME280::BME280(void)
{
    settings.i2c_addr = 0x76;
    settings.temp_correction = 0.0;
}

/* i2c read/write register */
uint8_t BME280::read_reg(uint8_t offset)
{
    uint8_t result = 0;

    Wire.beginTransmission(settings.i2c_addr);
    Wire.write(offset);
    Wire.endTransmission();

    Wire.requestFrom(settings.i2c_addr, 1);
    while (Wire.available())
    {
        result = Wire.read();
    }

    return result;
}
void BME280::read_reg_range(uint8_t *output, uint8_t offset, uint8_t length)
{
    uint8_t counter = 0;
    char out = 0;

    Wire.beginTransmission(settings.i2c_addr);
    Wire.write(offset);
    Wire.endTransmission();

    Wire.requestFrom(settings.i2c_addr, length);
    while ((Wire.available()) && (counter < length))
    {
        out = Wire.read();
        *output = out;
        output++;
        counter++;
    }
}
void BME280::write_reg(uint8_t offset, uint8_t write_data)
{
    Wire.beginTransmission(settings.i2c_addr);
    Wire.write(offset);
    Wire.write(write_data);
    Wire.endTransmission();
}

uint16_t BME280::concat_bytes(uint8_t msb, uint8_t lsb)
{
    return (((uint16_t)msb << 8) | (uint16_t)lsb);
}

/* Begin and Initialize IC*/
void BME280::begin()
{
    Wire.begin();

    /* Get & Parse Calibration Data */
    get_calib_data();
}
uint8_t BME280::begin_i2c(TwoWire &wirePort)
{
    uint8_t chip_id = get_chip_id();

    if (chip_id == 0x60)
    {
        begin();
        return chip_id;
    }
    return 0xff;
}

/* Device Functions */
uint8_t BME280::get_chip_id()
{
    return read_reg(bme280_chip_id_addr);
}
void BME280::get_calib_data()
{

    /* Read Temperature Calibration Data */
    read_reg_range(calibration.temp_reg_data, bme280_dig_T1_lsb_reg, 6);

    /* Read Pressure Calibration Data */
    read_reg_range(calibration.pres_reg_data, bme280_dig_P1_lsb_reg, 18);

    /*Read Humidity Calibration Data */
    calibration.humid_reg_data_a = read_reg(bme280_dig_H1_lsb_reg);
    read_reg_range(calibration.humid_reg_data_b, bme280_dig_H2_lsb_reg, 7);

    /* Parse Temperature Calibration Data */
    calibration.dig_T1 = concat_bytes(calibration.temp_reg_data[1], calibration.temp_reg_data[0]);
    calibration.dig_T2 = (int16_t)concat_bytes(calibration.temp_reg_data[3], calibration.temp_reg_data[2]);
    calibration.dig_T3 = (int16_t)concat_bytes(calibration.temp_reg_data[5], calibration.temp_reg_data[4]);

    /* Parse Pressure Calibration Data */
    calibration.dig_P1 = concat_bytes(calibration.pres_reg_data[1], calibration.pres_reg_data[0]);
    calibration.dig_P2 = (int16_t)concat_bytes(calibration.pres_reg_data[3], calibration.pres_reg_data[2]);
    calibration.dig_P3 = (int16_t)concat_bytes(calibration.pres_reg_data[5], calibration.pres_reg_data[4]);
    calibration.dig_P4 = (int16_t)concat_bytes(calibration.pres_reg_data[7], calibration.pres_reg_data[6]);
    calibration.dig_P5 = (int16_t)concat_bytes(calibration.pres_reg_data[9], calibration.pres_reg_data[8]);
    calibration.dig_P6 = (int16_t)concat_bytes(calibration.pres_reg_data[11], calibration.pres_reg_data[10]);
    calibration.dig_P7 = (int16_t)concat_bytes(calibration.pres_reg_data[13], calibration.pres_reg_data[12]);
    calibration.dig_P8 = (int16_t)concat_bytes(calibration.pres_reg_data[15], calibration.pres_reg_data[14]);
    calibration.dig_P9 = (int16_t)concat_bytes(calibration.pres_reg_data[17], calibration.pres_reg_data[16]);

    /* Parse Humidity Calibration Data */
    int16_t dig_H4_msb;
    int16_t dig_H4_lsb;
    int16_t dig_H5_msb;
    int16_t dig_H5_lsb;

    calibration.dig_H1 = calibration.humid_reg_data_a;
    calibration.dig_H2 = (int16_t)concat_bytes(calibration.humid_reg_data_b[1], calibration.humid_reg_data_b[0]);
    calibration.dig_H3 = calibration.humid_reg_data_b[2];

    dig_H4_msb = (int16_t)(int8_t)calibration.humid_reg_data_b[3] * 16;
    dig_H4_lsb = (int16_t)(calibration.humid_reg_data_b[4] & 0x0F);
    calibration.dig_H4 = dig_H4_msb | dig_H4_lsb;

    dig_H5_msb = (int16_t)(int8_t)calibration.humid_reg_data_b[5] * 16;
    dig_H5_lsb = (int16_t)(calibration.humid_reg_data_b[4] >> 4);
    calibration.dig_H5 = dig_H5_msb | dig_H5_lsb;

    calibration.dig_H6 = (int8_t)calibration.humid_reg_data_b[6];
}
uint8_t BME280::get_mode()
{
}
/* Set Sleep Mode */
void BME280::set_mode(uint8_t mode)
{
    if (mode > 0b11)
    {
        mode = 0b00;
    }

    uint8_t reg_value = read_reg(bme280_ctrl_meas_addr);
    reg_value &= ~(1 << 0); // Clear bit 0
    reg_value &= ~(1 << 1); // Clear bit 1
    reg_value |= mode;
    write_reg(bme280_ctrl_meas_addr, reg_value);
}

/* Set Temperature Oversampling */
void BME280::set_temp_osrs(uint8_t osrs_value)
{
    uint8_t reg_value = read_reg(bme280_ctrl_meas_addr);
    reg_value &= ~(1 << 5); // Clear bit 5
    reg_value &= ~(1 << 6); // Clear bit 6
    reg_value &= ~(1 << 7); // Clear bit 7
    reg_value |= osrs_value << 5;
    write_reg(bme280_ctrl_meas_addr, reg_value);
}
/* Set Pressure Oversampling */
void BME280::set_pres_osrs(uint8_t osrs_value)
{
    uint8_t reg_value = read_reg(bme280_ctrl_meas_addr);
    reg_value &= ~(1 << 2); // Clear bit 2
    reg_value &= ~(1 << 3); // Clear bit 3
    reg_value &= ~(1 << 4); // Clear bit 4
    reg_value |= osrs_value << 2;
    write_reg(bme280_ctrl_meas_addr, reg_value);
}
/* Set Humidity Oversampling */
void BME280::set_humid_osrs(uint8_t osrs_value)
{
    uint8_t reg_value = read_reg(bme280_ctrl_hum_addr);
    reg_value &= ~(1 << 0); // Clear bit 0
    reg_value &= ~(1 << 1); // Clear bit 1
    reg_value &= ~(1 << 2); // Clear bit 2
    reg_value |= osrs_value << 0;
    write_reg(bme280_ctrl_hum_addr, reg_value);
}
/* Set Standby Time */
void BME280::set_standby_time(uint8_t time_setting)
{
    uint8_t reg_value = read_reg(bme280_config_addr);
    reg_value &= ~(1 << 5); // Clear bit 5
    reg_value &= ~(1 << 6); // Clear bit 6
    reg_value &= ~(1 << 7); // Clear bit 7
    reg_value |= time_setting << 5;
    write_reg(bme280_config_addr, reg_value);
}
/* Set Filter */
void BME280::set_filter(uint8_t filter_setting)
{
    uint8_t reg_value = read_reg(bme280_config_addr);
    reg_value &= ~(1 << 2); // Clear bit 2
    reg_value &= ~(1 << 3); // Clear bit 3
    reg_value &= ~(1 << 4); // Clear bit 4
    reg_value |= filter_setting << 2;
    write_reg(bme280_config_addr, reg_value);
}
/* Read Pressure */
float BME280::read_float_pres()
{
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;
    uint32_t uncomp_pres;
    double var1;
    double var2;
    double p;
    uint8_t buff[3];

    read_reg_range(buff, bme280_press_msb_addr, 3);

    data_msb = (uint32_t)buff[0] << 12;
    data_lsb = (uint32_t)buff[1] << 4;
    data_xlsb = (uint32_t)buff[2] >> 4;
    uncomp_pres = data_msb | data_lsb | data_xlsb;

    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)calibration.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calibration.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)calibration.dig_P4) * 65536.0);
    var1 = (((double)calibration.dig_P3) * var1 * var1 / 524288.0 + ((double)calibration.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)calibration.dig_P1);

    if (var1 == 0.0)
    {
        return 0;
    }

    p = 1048576.0 - (double)uncomp_pres;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)calibration.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)calibration.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)calibration.dig_P7)) / 16.0;

    return p / 100;
}

/* Read Humidity */
float BME280::read_float_humidity()
{
    double var_H;
    uint32_t data_lsb;
    uint32_t data_msb;
    uint32_t uncomp_humid;
    uint8_t buff[2];

    read_reg_range(buff, bme280_hum_msb_addr, 2);

    data_msb = (uint32_t)buff[0] << 8;
    data_lsb = (uint32_t)buff[1];

    uncomp_humid = data_msb | data_lsb;

    var_H = (((double)t_fine) - 76800.0);
    var_H = (uncomp_humid - (((double)calibration.dig_H4) * 64.0 + ((double)calibration.dig_H5) / 16384.0 * var_H)) *
            (((double)calibration.dig_H2) / 65536.0 * (1.0 + ((double)calibration.dig_H6) / 67108864.0 * var_H * (1.0 + ((double)calibration.dig_H3) / 671008864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)calibration.dig_H1) * var_H / 524288.0);

    return var_H;
}
/* Read Temperatiure in Celsius */
float BME280::read_float_temp_c()
{

    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;
    uint32_t uncomp_temp;
    double var1;
    double var2;
    double temp;
    uint8_t buff[3];

    read_reg_range(buff, bme280_temp_msb_addr, 3);

    data_msb = (uint32_t)buff[0] << 12;
    data_lsb = (uint32_t)buff[1] << 4;
    data_xlsb = (uint32_t)buff[2] >> 4;

    uncomp_temp = data_msb | data_lsb | data_xlsb;

    var1 = ((double)uncomp_temp) / 16384.0 - ((double)calibration.dig_T1) / 1024.0;
    var1 = var1 * ((double)calibration.dig_T2);
    var2 = (((double)uncomp_temp) / 131072.0 - ((double)calibration.dig_T1) / 8192.0);
    var2 = (var2 * var2) * ((double)calibration.dig_T3);
    t_fine = (int32_t)(var1 + var2);
    temp = (var1 + var2) / 5120.0;

    return temp;
}

/* Read Temperature in Farenheit */
float BME280::read_float_temp_f()
{
    double temp_f;
    double temp_c;

    temp_c = read_float_temp_c();
    temp_f = (temp_c * 1.8) + 32.0;

    return temp_f;
}