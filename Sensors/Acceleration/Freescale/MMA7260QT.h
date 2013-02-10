/*
 * $Id: MMA7260QT.h,v 1.5 2010/07/15 14:38:19 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: MMA7260QT.h,v $
 * Revision 1.5  2010/07/15 14:38:19  clivewebster
 * Allow variable ADC reference voltage
 *
 * Revision 1.4  2010/06/14 18:50:17  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/01/24 17:10:16  clivewebster
 * Use ADC channels in constructor rather than IOPins
 *
 * Revision 1.2  2009/12/11 17:17:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.1  2009/11/01 00:50:25  clivewebster
 * *** empty log message ***
 *
 * ===========
 *
 * Copyright (C) 2010 Clive Webster (webbot@webbot.org.uk)
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
 *
 *
 * MMA7260QT.h
 *
 *  Created on: 28-Oct-2009
 *      Author: Clive Webster
 *
 *  Freescale MMA7260QT 3 axis accelerometer
 *
 *  Operates at 2v2 to 3v6 supply.
 *
 *  Raw datasheet: http://www.pololu.com/file/download/MMA7260QT.pdf?file_id=0J87
 *
 *  However: for 5v projects you can buy a carrier board with on-board 3v3 regulator at
 *  http://www.pololu.com/catalog/product/766
 *
 *  This device has 2 input pins 'g-select1' and 'g-select2' that allow you to change the sensitivity
 *  of the device.
 *  	g_select1	g_select2		maximum G	mV/G
 *      	0			0				1.5		800
 *      	1			0				2.0		600
 *      	0			1				4.0		300
 *      	1			1				6.0		200
 *
 *   These pins could be set by output pins from your microcontroller but beware!
 *   If the mcu output pins are 5v then you will 'blow' the device as it is working at 3v3.
 *   The pololu board gets around this by adding pullup resistors to these pins. So the default
 *   is 6g. You can connect the pins to a microcontroller and control each of them as follows:
 *   - For a 0 then make your mcu an output pin and set it low
 *   - For a 1 then make your mcu an input pin and the on-board pullup will make it high
 *   NEVER make your mcu an output pin and set it high otherwise the 5v signal will fry the device.
 *
 *   The Pololu board provides jumpers that can be used to pull each of the select pins low, or left off,
 *   to make the pin high. This is a much better solution as it is doubtful you would want to keep
 *   changing the sensitivity at run time.
 *
 */

#ifndef MMA7260QT_H_
#define MMA7260QT_H_

#include "../_acceleration_common.h"
#include "../../../a2d.h"

// The class used to read a raw value
extern ACCELEROMETER_CLASS c_FS_MMA7260QT;

typedef struct s_FS_MMA7260QT{
	ACCELEROMETER accelerometer;
	ADC_CHANNEL  x_pin;
	ADC_CHANNEL  y_pin;
	ADC_CHANNEL  z_pin;
	uint8_t		 mode;
}FS_MMA7260QT;

// Create the sensor using X, Y and Z pins for 1.5g
#define MAKE_FS_MMA7260QT_1g5(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_FS_MMA7260QT), \
	pinX,\
	pinY,\
	pinZ, \
	0}

// Create the sensor using X, Y and Z pins for 2g
#define MAKE_FS_MMA7260QT_2g(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_FS_MMA7260QT), \
	pinX,\
	pinY,\
	pinZ, \
	1}

// Create the sensor using X, Y and Z pins for 4g
#define MAKE_FS_MMA7260QT_4g(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_FS_MMA7260QT), \
	pinX,\
	pinY,\
	pinZ, \
	2}

// Create the sensor using X, Y and Z pins for 6g
#define MAKE_FS_MMA7260QT_6g(pinX, pinY, pinZ) { \
	MAKE_ACCELEROMETER_SENSOR(c_FS_MMA7260QT), \
	pinX,\
	pinY,\
	pinZ, \
	3}


#endif /* MMA7260QT_H_ */
