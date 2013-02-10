/*
 * $Id: BMP085.c,v 1.1 2011/08/13 11:28:35 clivewebster Exp $
 * Revision History
 * ================
 * $Log: BMP085.c,v $
 * Revision 1.1  2011/08/13 11:28:35  clivewebster
 * Added
 *
 * ================
 *
 * Copyright (C) 2011 Clive Webster (webbot@webbot.org.uk)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *        File: BMP085.c
 *  Created on: 12 Aug 2011
 *      Author: Clive Webster
 */
#include "BMP085.h"
#include "../../../timer.h"

// #define TEST_CALC
/*
 *
 *	register definitions
 *
 */

#define BMP085_PROM_START__ADDR		0xaa
#define BMP085_PROM_DATA__LEN		  22

#define BMP085_CHIP_ID_REG			0xD0
#define BMP085_VERSION_REG			0xD1

#define BMP085_CTRL_MEAS_REG		0xF4
#define BMP085_ADC_OUT_MSB_REG		0xF6
#define BMP085_ADC_OUT_LSB_REG		0xF7

#define BMP085_SOFT_RESET_REG		0xE0

#define BMP085_T_MEASURE        0x2E				// temperature measurement
#define BMP085_P_MEASURE        0x34				// pressure measurement

#define BMP085_TEMP_CONVERSION_TIME  5				// 4.5ms



// Read the raw temperature value
static unsigned short bmp085_get_ut(const BMP085* device){
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	unsigned short ut=0;
	uint8_t data[2];

	if(i2cMasterWriteRegister(i2c, BMP085_CTRL_MEAS_REG, BMP085_T_MEASURE)){
		delay_ms(BMP085_TEMP_CONVERSION_TIME);
		if(i2cMasterReadRegisters(i2c,BMP085_ADC_OUT_MSB_REG,sizeof(data),data)){
		  ut = (data[0] <<8) | data[1];
		}
	}

	return ut;
}

static short bmp085_get_temperature(BMP085* device, unsigned long ut){
	short temperature;
	long x1,x2;

	x1 = (((long) ut - (long) device->cal_param.ac6) * (long) device->cal_param.ac5) >> 15;
	x2 = ((long) device->cal_param.mc << 11) / (x1 + device->cal_param.md);
	device->param_b5 = x1 + x2;

	temperature = ((device->param_b5 + 8) >> 4);  // temperature in 0.1°C

	return (temperature);
}


// Read the raw pressure value
static unsigned long bmp085_get_up(const BMP085* device){
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	unsigned long up=0;
	uint8_t data[3];

	if(i2cMasterWriteRegister(i2c, BMP085_CTRL_MEAS_REG, BMP085_P_MEASURE + (device->oss << 6))){
		delay_ms( 2 + (3 << (device->oss) ) );
		if(i2cMasterReadRegisters(i2c,BMP085_ADC_OUT_MSB_REG,sizeof(data),data)){
			up = (((unsigned long) data[0] << 16) | ((unsigned long) data[1] << 8) | (unsigned long) data[2]) >> (8 - device->oss);
		}
	}
	return up;
}

#define SMD500_PARAM_MG      3038        //calibration parameter
#define SMD500_PARAM_MH     -7357        //calibration parameter
#define SMD500_PARAM_MI      3791        //calibration parameter

static long bmp085_get_pressure(const BMP085* device, unsigned long up){
	long pressure,x1,x2,x3,b3,b6;
	unsigned long b4, b7;

	b6 = device->param_b5 - 4000;

	//*****calculate B3************
	x1 = (b6*b6) >> 12;
	x1 *= device->cal_param.b2;
	x1 >>=11;

	x2 = (device->cal_param.ac2*b6);
	x2 >>=11;

	x3 = x1 +x2;

	b3 = (((((long)device->cal_param.ac1 )*4 + x3) << device->oss) + 2) >> 2;

	//*****calculate B4************
	x1 = (device->cal_param.ac3* b6) >> 13;
	x2 = (device->cal_param.b1 * ((b6*b6) >> 12) ) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (device->cal_param.ac4 * (unsigned long) (x3 + 32768)) >> 15;

	b7 = ((unsigned long)(up - b3) * (50000>>device->oss));
	if (b7 < 0x80000000){
		pressure = (b7 << 1) / b4;
	}else{
		pressure = (b7 / b4) << 1;
	}

	x1 = pressure >> 8;
	x1 *= x1;
	x1 = (x1 * SMD500_PARAM_MG) >> 16;
	x2 = (pressure * SMD500_PARAM_MH) >> 16;
	pressure += (x1 + x2 + SMD500_PARAM_MI) >> 4;	// pressure in Pa

	return (pressure);
}



static void read(SENSOR* sensor){
	BMP085* device = (BMP085*)sensor;
	if(device->present){
#ifdef TEST_CALC
		// Read the raw values
		unsigned short ut = 27898;
		unsigned long  up = 23843;
#else
		// Read the raw values
		unsigned short ut = bmp085_get_ut(device);
		unsigned long  up = bmp085_get_up(device);
#endif
		// Convert
		short celsius  = bmp085_get_temperature(device, ut);
		long  pressure = bmp085_get_pressure(device, up);

		// Store it
		device->pressure.pa = pressure;
		device->celsius = ( celsius + 5 )/ 10;
	}
}

static void init(SENSOR* sensor){
	BMP085* device = (BMP085*)sensor;

#ifdef TEST_CALC
	 device->cal_param.ac1 =  408;
	 device->cal_param.ac2 =  -72;
	 device->cal_param.ac3 =  -14383;
	 device->cal_param.ac4 =  32741;
	 device->cal_param.ac5 =  32757;
	 device->cal_param.ac6 =  23153;

	 /*parameters B1,B2*/
	 device->cal_param.b1 =  6190;
	 device->cal_param.b2 =  4;

	 /*parameters MB,MC,MD*/
	 device->cal_param.mb =  -32768;
	 device->cal_param.mc =  -8711;
	 device->cal_param.md =  2868;

	 device->present = TRUE;
#else
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	uint8_t data[BMP085_PROM_DATA__LEN];

	// Read the calibration data
	if(i2cMasterReadRegisters(i2c,BMP085_PROM_START__ADDR,sizeof(data),data)){
		 /*parameters AC1-AC6*/
		 device->cal_param.ac1 =  (data[0] <<8) | data[1];
		 device->cal_param.ac2 =  (data[2] <<8) | data[3];
		 device->cal_param.ac3 =  (data[4] <<8) | data[5];
		 device->cal_param.ac4 =  (data[6] <<8) | data[7];
		 device->cal_param.ac5 =  (data[8] <<8) | data[9];
		 device->cal_param.ac6 =  (data[10] <<8) | data[11];

		 /*parameters B1,B2*/
		 device->cal_param.b1 =  (data[12] <<8) | data[13];
		 device->cal_param.b2 =  (data[14] <<8) | data[15];

		 /*parameters MB,MC,MD*/
		 device->cal_param.mb =  (data[16] <<8) | data[17];
		 device->cal_param.mc =  (data[18] <<8) | data[19];
		 device->cal_param.md =  (data[20] <<8) | data[21];

		 device->present = TRUE;
	}
#endif


	 // Store the value at power on
	 read(sensor);
	 device->pressure.paStart = device->pressure.pa;

}

// 10ms startup time
PRESSURE_CLASS const PROGMEM c_BMP085 = MAKE_PRESSURE_CLASS(&init,&read,10,0);



