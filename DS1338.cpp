#include "DS1338.h"
#include "Arduino.h"

extern "C" {
#include <inttypes.h>
}

/**
 * \defgroup ds1338_rtctime_t Time Structure and Associated Methods
 */

/** 
 * \brief Set the values in a struct rtctime_t instance. 
 * 
 * \param time A pointer to a struct rtctime_t instance. 
 * \param year The year.
 * \param month The month. 
 * \param day The day. 
 * \param hour The hour. 
 * \param minute The minute. 
 * \param second The second. 
 */
void make_time(struct rtctime_t *time, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	time->year = year;
	time->month = month;
	time->day = day;
	time->hour = hour;
	time->minute = minute;
	time->second = second;
}

/**
 * \brief Output a string representation of a struct rtctime_t object.
 *
 * Requires a buffer size of at least 20 characters. 
 *
 * \param time A pointer to a struct rtctime_t instance. 
 * \param buf A pointer to a character buffer in which to store the string.
 */
void format_time_str(struct rtctime_t *time, char *buf)
{
	// Year
	buf[0] = '2';
	if (time->year < 10)
	{
		buf[1] = '0';
		buf[2] = '0';
		buf[3] = '0' + time->year;
	}
	else if (time->year < 100)
	{
		buf[1] = '0';
		buf[2] = '0' + (time->year / 10);
		buf[3] = '0' + (time->year % 10);
	}
	else
	{
		buf[1] = '1';
		buf[2] = '0' + ((time->year - 100) / 10);
		buf[3] = '0' + ((time->year - 100) % 10);
	}
	buf[4] = '-';

	// Month
	if (time->month < 10)
	{
		buf[5] = '0';
		buf[6] = '0' + time->month;
	}
	else
	{
		buf[5] = '0' + (time->month / 10);
		buf[6] = '0' + (time->month % 10);
	}
	buf[7] = '-';

	// Day
	if (time->day < 10)
	{
		buf[8] = '0';
		buf[9] = '0' + time->day;
	}
	else
	{
		buf[8] = '0' + (time->day / 10);
		buf[9] = '0' + (time->day % 10);
	}
	buf[10] = 'T';

	// Hour
	if (time->hour < 10)
	{
		buf[11] = '0';
		buf[12] = '0' + time->hour;
	}
	else
	{
		buf[11] = '0' + (time->hour / 10);
		buf[12] = '0' + (time->hour % 10);
	}
	buf[13] = ':';

	// Minute
	if (time->minute < 10)
	{
		buf[14] = '0';
		buf[15] = '0' + time->minute;
	}
	else
	{
		buf[14] = '0' + (time->minute / 10);
		buf[15] = '0' + (time->minute % 10);
	}
	buf[16] = ':';

	// Second
	if (time->second < 10)
	{
		buf[17] = '0';
		buf[18] = '0' + time->second;
	}
	else
	{
		buf[17] = '0' + (time->second / 10);
		buf[18] = '0' + (time->second % 10);
	}

	buf[19] = 0;
}

/**
 * \defgroup ds1338_i2c I2C Functions
 */

/**
 * \brief Read data from an I2C device. 
 *
 * \param addr The address of the device from which to read. 
 * \param buf A pointer to a buffer in which to store the data. 
 * \param num The number of bytes to read. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t i2c_read(uint8_t addr, uint8_t *buf, uint8_t num)
{
	Wire.requestFrom(addr, num);

	if (Wire.available() < num)
	{
		return READ_ERROR;
	}

	for (uint8_t i = 0; i < num; i++)
	{
		buf[i] = Wire.read();
	}

	return 0;
}

/**
 * \brief Write data to an I2C device. 
 *
 * \param addr The address of the device to which to write. 
 * \param buf A pointer to a buffer from which to read the data. 
 * \param num The number of bytes to write. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t i2c_write(uint8_t addr, uint8_t *buf, uint8_t num)
{
	Wire.beginTransmission(addr);
	for (uint8_t i = 0; i < num; i++)
	{
		Wire.write(buf[i]);
	}
	return Wire.endTransmission();
}

static uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t num)
{
	Wire.beginTransmission(addr);
	Wire.write(reg);
	for (uint8_t i = 0; i < num; i++)
	{
		Wire.write(buf[i]);
	}
	return Wire.endTransmission();
}

/**
 * \brief Write a single byte to an I2C device. 
 *
 * \param addr The address of the device to which to write. 
 * \param b The byte to write. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t i2c_write_1(uint8_t addr, uint8_t b)
{
	Wire.beginTransmission(addr);
	Wire.write(b);
	return Wire.endTransmission();
}

/**
 * \defgroup ds1338_time Time Getter and Setter Methods
 */

/** 
 * \brief Read the current time. 
 * 
 * \param time A pointer to a struct rtctime_t instance in which to store the time. 
 * 
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1338_read_time(struct rtctime_t *time)
{
	uint8_t buf[7];

	uint8_t res = i2c_write_1(DS1338_ADDR, DS1338_REG_SECONDS);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return 1;
	}

	res = i2c_read(DS1338_ADDR, buf, 7);

	if (res)
	{
		pln("I2C read error.");
		return 2;
	}

	time->second = decode_bcd(buf[0]);
	time->minute = decode_bcd(buf[1]);

	if (buf[2] & DS1338_HOUR_12)
	{
		time->hour = ((buf[2] >> 4) & 0x01) * 12 + ((buf[2] >> 5) & 0x01) * 12;
	}
	else
	{
		time->hour = decode_bcd(buf[2]);
	}

	time->day = decode_bcd(buf[4]);
	time->month = decode_bcd(buf[5] & 0x1F);
	time->year = 100 * ((buf[5] >> 7) & 0x01) + decode_bcd(buf[6]);

	return 0;
}

/** 
 * \brief Set the time. 
 * 
 * \param time A pointer to a struct rtctime_t instance containing the time to set. 
 * 
 * \return 0 on success; otherwise an I2C error.
 */
uint8_t ds1338_write_time(struct rtctime_t *time)
{
	uint8_t buf[8];

	buf[0] = DS1338_REG_SECONDS;
	buf[1] = encode_bcd(time->second);
	buf[2] = encode_bcd(time->minute);
	buf[3] = encode_bcd(time->hour); // Time always stored in 24-hour format
	buf[4] = 1;						 // Not used
	buf[5] = encode_bcd(time->day);
	buf[6] = ((time->year / 100) << 7) | encode_bcd(time->month);
	buf[7] = encode_bcd((time->year) % 100);

	uint8_t res = i2c_write(DS1338_ADDR, buf, 8);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return 1;
	}

	return 0;
}

/**
 * \defgroup ds1338_control Control Register Methods
 */

/**
 * \brief Get the value of the control register.
 *
 * \param ctrl A pointer to a value in which to store the value of the control register. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t ds1338_get_control(uint8_t *ctrl)
{
	uint8_t res = i2c_write_1(DS1338_ADDR, DS1338_REG_CONTROL);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return res;
	}

	res = i2c_read(DS1338_ADDR, ctrl, 1);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return res;
	}

	return 0;
}

/**
 * \brief Set the value of the control register.
 *
 * \param ctrl The value to set. 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t ds1338_set_control(uint8_t ctrl)
{
	uint8_t buf[2];
	buf[0] = DS1338_REG_CONTROL;
	buf[1] = ctrl;
	return i2c_write(DS1338_ADDR, buf, 2);
}

/**
 * \brief Set the specified bits in the control register.
 *
 * \param mask A mask specifying which bits to set. (High bits will be set.)
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t ds1338_set_control_bits(uint8_t mask)
{ // set bits
	uint8_t ctrl;
	uint8_t res = ds1338_get_control(&ctrl);
	if (res)
	{
		return res;
	}
	ctrl |= mask;
	return ds1338_set_control(ctrl);
}

/**
 * \brief Clear the specified bits in the control register.
 *
 * \param mask A mask specifying which bits to clear. (High bits will be cleared.) 
 *
 * \return 0 on success; otherwise an I2C error.
 */
static uint8_t ds1338_clear_control_bits(uint8_t mask)
{
	return ds1338_set_control(~mask);
}

uint8_t ds1338_get_osf(uint8_t *osf)
{
	uint8_t ctrl;
	uint8_t res = ds1338_get_control(&ctrl);
	if (res)
	{
		return res;
	}
	if (ctrl & DS1338_OSC_STOP_FLAG == 0)
	{
		*osf = 0;
	}
	else
	{
		*osf = 1;
	}
	return 0;
}

uint8_t ds1338_clean_osf()
{
	return ds1338_clear_control_bits(DS1338_OSC_STOP_FLAG);
}

/*
#define DS1338_SQWE_FLAG (0x01 << 4)
#define DS1338_SQW_MASK (0x03)
#define DS1338_SQW_32768HZ (0x03)
#define DS1338_SQW_8192HZ (0x02)
#define DS1338_SQW_4096HZ (0x01)
#define DS1338_SQW_1HZ (0x00)
*/

uint8_t ds1338_enable_sqw(uint8_t frequence)
{
	uint8_t ctrl=0;
	frequence = frequence & DS1338_SQW_MASK;
	ctrl |= frequence;
	ctrl |= DS1338_SQWE_FLAG;
	ds1338_clear_control_bits(DS1338_SQW_MASK);
	return ds1338_set_control_bits(ctrl);
}

uint8_t ds1338_disable_sqw()
{
	return ds1338_clear_control_bits(DS1338_SQWE_FLAG|DS1338_SQW_MASK);
}

uint8_t ds1338_read_ram(uint8_t address, uint8_t *data, uint8_t length)
{
	if (address > DS1338_REG_RAM_END || address < DS1338_REG_RAM_BEGIN)
	{
		return 1;
	}

	if (address + length > DS1338_REG_RAM_END)
	{
		return 1;
	}

	uint8_t res = i2c_write_1(DS1338_ADDR, address);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return res;
	}

	res = i2c_read(DS1338_ADDR, data, length);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return res;
	}

	return 0;
}

uint8_t ds1338_write_ram(uint8_t address, uint8_t *data, uint8_t length)
{
	if (address > DS1338_REG_RAM_END || address < DS1338_REG_RAM_BEGIN)
	{
		return 1;
	}

	if (address + length > DS1338_REG_RAM_END)
	{
		return 1;
	}

	uint8_t res = i2c_write_reg(DS1338_ADDR, address, data, length);

	if (res)
	{
		p("I2C write error: ");
		pln(res, DEC);
		return 1;
	}

	return 0;
}
