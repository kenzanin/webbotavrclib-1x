/*
 * $Id: L298.h,v 1.8 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: L298.h,v $
 * Revision 1.8  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.7  2011/07/05 23:58:23  clivewebster
 * SOLAR_L298_MOTOR_LIST now requires PROGMEM
 *
 * Revision 1.6  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/01/24 13:56:24  clivewebster
 * *** empty log message ***
 *
 * Revision 1.4  2009/12/11 17:12:56  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.3  2009/11/02 18:33:32  clivewebster
 * Added revision log
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
 * L298.h
 *
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Support for the L298 dc dual-motor driver
 *  Available from Solarbotics
 *
 *  Operating range: 6v - 26v
 *  Current: 2.5 amp continuous per channel, 3 amp peak
 */

#ifndef SOLAR_L298_H_
#define SOLAR_L298_H_
#include "../../actuators.h"

/* -------------------------------------------------------
 * Define the structure for one motor
-------------------------------------------------------- */
typedef struct s_l298_motor{
	__ACTUATOR 	actuator;	// has all the common stuff
	const IOPin *pin;		// The IO pin the motor is connected to - must be a timer compare pin
	const IOPin *enable1;	// The IO pin used to set the direction of the motor
	const IOPin *enable2;	// The IO pin used to set the direction of the motor
} SOLAR_L298_MOTOR;

// Define the constructor for a SOLAR_L298 motor
// Usage: SOLAR_L298_MOTOR motor1 = MAKE_SOLAR_L298_MOTOR(FALSE, xx, yy, zz);
// The motor pin must a timer compare output pin whereas the enable pins can be any IO pin
// MOTOR_MODE_LH_HL_LLH
#define MAKE_SOLAR_L298_MOTOR(inverted, motorPin, enable1, enable2)  { MAKE_ACTUATOR(inverted),motorPin,enable1,enable2 }

/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef SOLAR_L298_MOTOR* /*PROGMEM*/  SOLAR_L298_MOTOR_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
-------------------------------------------------------- */
typedef struct s_solar_l298_driver{
	SOLAR_L298_MOTOR_LIST* 	const motors;		// The motors
	uint8_t 				num_motors; 		// The total number of motors in the list
} SOLAR_L298_MOTOR_DRIVER;

#define MAKE_SOLAR_L298_MOTOR_DRIVER(motorlst) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(SOLAR_L298_MOTOR*)) \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */
void solarL298Init(SOLAR_L298_MOTOR_DRIVER* driver, uint32_t freq);



#endif /* SOLAR_L298_H_ */
