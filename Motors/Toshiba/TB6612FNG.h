/*
 * $Id: TB6612FNG.h,v 1.7 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: TB6612FNG.h,v $
 * Revision 1.7  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.6  2011/07/06 00:08:10  clivewebster
 * TOSHIBA_TB6612FNG_3pin_MOTOR_LIST and TOSHIBA_TB6612FNG_2pin_MOTOR_LIST requires PROGMEM
 *
 * Revision 1.5  2010/06/14 18:46:56  clivewebster
 * Add copyright license info
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
 * TB6612FNG.h
 *
 *  Created on: 19-Sep-2009
 *      Author: Clive Webster
 *
 *  Support for the TB6612FNG dc dual-motor driver.
 *  Available from pololu http://www.pololu.com/catalog/product/713/resources
 *  Datasheet: http://www.pololu.com/file/0J86/TB6612FNG.pdf
 *
 *  Operating range: 4.5v - 13.5v
 *  Current: 1 amp continuous per channel, 3 amp peak
 *
 *  Motor1:
 *  	In1: 21
 *  	In2: 22
 *  	PwmIn: 23
 *  	Output pins: 1/2 and 56
 *
 *  Motor2:
 *  	In1: 17
 *  	In2: 16
 *  	PwmIn: 15
 *  	Output pins: 11/12 and 7/8
 *
 *  The 'standby' input on pin 19 should be connected to Vcc otherwise the device is
 *  permanently on stand by
 *
 *  Logic truth table:
 *  In1		In2		PwmIn		Output
 *  H		H		x		brake
 *  L		H		H		Reverse
 *  L		H		L		brake
 *  H		L		H		Forward
 *  H		L		L		brake
 *  L		L		H		coast
 *
 *  So there are two ways to control the device for variable speeds:-
 *
 *  Either: one PWM output pin connected to 'PwmIn' and 2 digital I/Os for In1 and In2
 *
 *  In1		In2		PwmIn	Output
 *  L		H		pwm		Reverse/brake
 *  H		L		pwm		Forward/brake
 *  H		H		pwm		brake
 *  L		L		H		coast
 *
 *  Or: two PWM output pins connected to 'In1' and 'In2' and PwmIn permanently connected to Vcc.
 *  Of the two PWM outputs then only one is pwm'd at any time whilst the other is set high.
 *  In1		In2		PwmIn	Output
 *  pwm		H		H		Reverse/brake
 *  H		pwm		H		Forward/brake
 *  H		H		H		brake
 *  L		L		H		coast
 *
 *
 */

#ifndef TB6612FNG_H_
#define TB6612FNG_H_

#include "../../actuators.h"

/* -------------------------------------------------------
 * Define the structure for one motor using 3 pins
-------------------------------------------------------- */
typedef struct s_toshiba_tb6612fng_3pin{
	__ACTUATOR 	actuator;	// has all the common stuff
	const IOPin *pwm;		// The IO pin for pwm
	const IOPin *enable1;	// The IO pin used to set the direction of the motor
	const IOPin *enable2;	// The IO pin used to set the direction of the motor
} TOSHIBA_TB6612FNG_3pin_MOTOR;

// Constructor for 3 pin
#define MAKE_TOSHIBA_TB6612FNG_MOTOR_3pin(inverted, pwm, enable1, enable2)  { MAKE_ACTUATOR(inverted),pwm,enable1,enable2 }

/* -------------------------------------------------------
 * Define the structure for one motor using 2 pwm pins
 * One pwm is active whilst the other pin is high
-------------------------------------------------------- */
typedef struct s_toshiba_tb6612fng_2pin{
	__ACTUATOR 	actuator;	// has all the common stuff
	const IOPin *pwm1;		// PWM input 1
	const IOPin *pwm2;		// PWM input 2

} TOSHIBA_TB6612FNG_2pin_MOTOR;

// Constructor for 3 pin
#define MAKE_TOSHIBA_TB6612FNG_MOTOR_2pin(inverted, pwm1, pwm2)  { MAKE_ACTUATOR(inverted),pwm1,pwm2 }

/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef TOSHIBA_TB6612FNG_3pin_MOTOR* /*PROGMEM*/  TOSHIBA_TB6612FNG_3pin_MOTOR_LIST;
typedef TOSHIBA_TB6612FNG_2pin_MOTOR* /*PROGMEM*/  TOSHIBA_TB6612FNG_2pin_MOTOR_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
 * using 3 pins
-------------------------------------------------------- */
typedef struct s_toshiba_tb6612fng_3pin_driver{
	TOSHIBA_TB6612FNG_3pin_MOTOR_LIST* 	const motors;		// The motors
	uint8_t 						    num_motors; 		// The total number of motors in the list
} TOSHIBA_TB6612FNG_3pin_MOTOR_DRIVER;

#define MAKE_TOSHIBA_TB6612FNG_3pin_MOTOR_DRIVER(motorlst) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(TOSHIBA_TB6612FNG_3pin_MOTOR*)) \
}

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
 * using 2 pins
-------------------------------------------------------- */
typedef struct s_toshiba_tb6612fng_2pin_driver{
	TOSHIBA_TB6612FNG_2pin_MOTOR_LIST* 	const motors;		// The motors
	uint8_t 						    num_motors; 		// The total number of motors in the list
} TOSHIBA_TB6612FNG_2pin_MOTOR_DRIVER;

#define MAKE_TOSHIBA_TB6612FNG_2pin_MOTOR_DRIVER(motorlst) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(TOSHIBA_TB6612FNG_2pin_MOTOR*)) \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */
void toshibaTB6612FNG_3pin_Init(TOSHIBA_TB6612FNG_3pin_MOTOR_DRIVER* driver, uint32_t freq);
void toshibaTB6612FNG_2pin_Init(TOSHIBA_TB6612FNG_2pin_MOTOR_DRIVER* driver, uint32_t freq);

#endif /* TB6612FNG_H_ */
