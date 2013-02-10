/*
 * $Id: ITG3200.c,v 1.3 2011/09/08 11:57:53 clivewebster Exp $
 * Revision History
 * ================
 * $Log: ITG3200.c,v $
 * Revision 1.3  2011/09/08 11:57:53  clivewebster
 * Fixed 'init' bug to accumulate total properly
 *
 * Revision 1.2  2011/01/25 23:49:38  clivewebster
 * Auto calibrate the zer position on start up
 *
 * Revision 1.1  2011/01/05 00:51:41  clivewebster
 * Added ITG3200 3 axis I2C gyro sensor
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
 *        File: ITG3200.c
 *  Created on: 3 Jan 2011
 *      Author: Clive Webster
 *
 *      Normal current = 6.5mA
 *      Supply = 2.1v to 3.6v
 */
#include "ITG3200.h"
#include "../../../timer.h"
#include "../../../fraction.h"

// Registers
#define WHO		0x00
#define	SMPL	0x15
#define DLPF	0x16
#define INT_C	0x17
#define INT_S	0x1A
#define	TMP_H	0x1B
#define	TMP_L	0x1C
#define	GX_H	0x1D
#define	GX_L	0x1E
#define	GY_H	0x1F
#define	GY_L	0x20
#define GZ_H	0x21
#define GZ_L	0x22
#define PWR_M	0x3E

// Same as dividing by 14.375
static const uint16_t PROGMEM frac[]     = {0, 15, 345, 0};


static void init(SENSOR* sensor){
	ITG3200* device = (ITG3200*)sensor;
	const I2C_DEVICE* i2c = &(device->i2cInfo);

	i2cMasterWriteRegister(i2c,INT_C, 0x05);	// Generate interrupt when device is ready or raw data ready
	i2cMasterWriteRegister(i2c,PWR_M, 0x80);	// Reset to defaults
	i2cMasterWriteRegister(i2c,SMPL, 0x00);		// SMLPRT_DIV = 0 (sample at 8kHz)
	i2cMasterWriteRegister(i2c,DLPF,0x18);		// DLPF_CFG = 0 (sample at 8kHz), FS_SEL = 3 (ie range = +- 2000 deg/sec
	i2cMasterWriteRegister(i2c,INT_C, 0x05);	// Generate interrupt when device is ready or raw data ready
	i2cMasterWriteRegister(i2c,PWR_M, 0x00);
	delay_ms(1000);

	// The default is that device should be outputing 0
	device->x_zero = 0;
	device->y_zero = 0;
	device->z_zero = 0;

	// Read 8 times
	uint8_t count=0;
	for(uint8_t i=0; i<8; i++){
		uint8_t response[6];
		if(i2cMasterReadRegisters(i2c,GX_H,sizeof(response),response)){
			int16_t tmp;
			tmp = (response[0] << 8) | response[1];	// x
			device->x_zero += tmp;

			tmp = (response[2] << 8) | response[3];	// y
			device->y_zero += tmp;

			tmp = (response[4] << 8) | response[5];	// z
			device->z_zero += tmp;

			count++;
		}
	}

	// Get average
	if(count){
		device->x_zero /= count;
		device->y_zero /= count;
		device->z_zero /= count;
	}
}

static void read(SENSOR* sensor){
	ITG3200* device = (ITG3200*)sensor;
	const I2C_DEVICE* i2c = &(device->i2cInfo);
	uint8_t response[8];
	if(i2cMasterReadRegisters(i2c,TMP_H,sizeof(response),response)){
		int16_t tmp;
		// tempC = 35.0 + ((rawVal + 13200)/280.0);
		tmp = (response[0] << 8) | response[1];	// temperature
		device->celsius = 35 + ((tmp + 13200)/280);

		// Divide readings by 14.375 to get degrees per second
		// Same as multiply by 0.069565217391304347826086956521739
		// Fib series: 0, 15, 345
		tmp = (response[2] << 8) | response[3];	// x
		tmp -= device->x_zero;
		device->gyro.x_axis_degrees_per_second = fraction32(tmp, frac);

		tmp = (response[4] << 8) | response[5];	// y
		tmp -= device->y_zero;
		device->gyro.y_axis_degrees_per_second = fraction32(tmp, frac);

		tmp = (response[6] << 8) | response[7];	// z
		tmp -= device->z_zero;
		device->gyro.z_axis_degrees_per_second = fraction32(tmp, frac);
	}
}

// Make the class, 50ms on startup, 0ms between readings
GYRO_CLASS c_ITG3200 = MAKE_GYRO_CLASS(&init,&read,50,0);
