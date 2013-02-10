/*
 * $Id: LB1836M.h,v 1.6 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: LB1836M.h,v $
 * Revision 1.6  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.5  2011/07/05 23:57:49  clivewebster
 * SANYO_LB1836M_MOTOR_LIST now requires PROGMEM
 *
 * Revision 1.4  2010/06/14 18:46:57  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2009/12/11 17:12:29  clivewebster
 * Fixed #include for Unix
 *
 * Revision 1.2  2009/11/02 18:33:32  clivewebster
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
 * LB1836M.h
 *
 *  Created on: 7-Oct-2009
 *      Author: Clive Webster
 *
 *  Support for the LB1836M dc dual-motor driver as used by the 'old' baby orangutan
 *
 *
 *  Logic truth table:
 *  In1		In2		Output
 *  H		L		Forward
 *  L		H		Reverse
 *  H		H		brake
 *  L		L		coast

 *
 *  Two PWM output pins connected to 'In1' and 'In2'
 *  Of the two PWM outputs then only one is pwm'd at any time whilst the other is set high.
 *  In1		In2		Output
 *  pwm		H		Reverse/brake
 *  H		pwm		Forward/brake
 *  H		H		brake
 *  L		L		coast
 *
 *
 */

#ifndef LB1836M_H_
#define LB1836M_H_

#include "../../actuators.h"

/* -------------------------------------------------------
 * Define the structure for one motor using 2 pwm pins
 * One pwm is active whilst the other pin is high
-------------------------------------------------------- */
typedef struct s_sanyo_lb1836m{
	__ACTUATOR 	actuator;	// has all the common stuff
	const IOPin *pwm1;		// PWM input 1
	const IOPin *pwm2;		// PWM input 2

} SANYO_LB1836M_MOTOR;

// Constructor
#define MAKE_SANYO_LB1836M_MOTOR(inverted, pwm1, pwm2)  { MAKE_ACTUATOR(inverted),pwm1,pwm2 }

/* -------------------------------------------------------
 * Define the data type to store a list of these motors
-------------------------------------------------------- */
typedef SANYO_LB1836M_MOTOR* /*PROGMEM*/  SANYO_LB1836M_MOTOR_LIST;

/* -------------------------------------------------------
 * Define the data type to control a list of these motors
 * using 2 pins
-------------------------------------------------------- */
typedef struct s_sanyo_lb1836m_driver{
	SANYO_LB1836M_MOTOR_LIST* 	const motors;		// The motors
	uint8_t 					num_motors; 		// The total number of motors in the list
} SANYO_LB1836M_MOTOR_DRIVER;

#define MAKE_SANYO_LB1836M_MOTOR_DRIVER(motorlst) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(SANYO_LB1836M_MOTOR*)) \
}

/* -------------------------------------------------------
 * Define methods specific to this driver
-------------------------------------------------------- */
void sanyoLB1836M_Init(SANYO_LB1836M_MOTOR_DRIVER* driver, uint32_t freq);

#endif /* LB1836M_H_ */
