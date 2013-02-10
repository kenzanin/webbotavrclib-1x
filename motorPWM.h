/*
 * $Id: motorPWM.h,v 1.11 2011/07/09 16:43:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: motorPWM.h,v $
 * Revision 1.11  2011/07/09 16:43:51  clivewebster
 * Allow user to specify PWM frequency for motors
 *
 * Revision 1.10  2011/07/06 00:29:33  clivewebster
 * Add PROGMEM to MOTOR_LIST
 *
 * Revision 1.9  2010/10/08 00:15:46  clivewebster
 * Add specific L293D DC motor 2 pin and 3 pin support
 *
 * Revision 1.8  2010/08/10 22:51:06  clivewebster
 * Allow both 2 pin (with tri state switch) or 3 pin for an h bridge like L293D
 *
 * Revision 1.7  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.6  2009/11/02 19:00:09  clivewebster
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
 * motorPWM.h
 *
 *  Created on: 24-Mar-2009
 *      Author: Clive Webster
 *
 *  A motor driver that uses one PWM for speed, and one output pin for direction.
 *  This includes my motor drivers at:
 *  http://www.societyofrobots.com/member_tutorials/node/159
 */

#ifndef MOTORPWM_H_
#define MOTORPWM_H_

#include "actuators.h"


typedef struct s_motor{
	__ACTUATOR 	actuator;		// has all the common stuff
	const IOPin *pwm;			// The PWM IO pin the motor is connected to - must be a timer compare pin
	const IOPin *direction1;	// The compulsory IO pin used to set the direction of the motor
	const IOPin *direction2;	// The optional IO pin that is normally NOT(direction1) unless breaking or coasting
} MOTOR;


// Define the standard constructor for a dc motor - default to 2 pin
#define MAKE_MOTOR_3_PIN(inverted, motorPin, directionPin1, directionPin2)  { MAKE_ACTUATOR(inverted),motorPin,directionPin1,directionPin2}
#define MAKE_MOTOR_2_PIN(inverted, motorPin, directionPin)  MAKE_MOTOR_3_PIN(inverted,motorPin,directionPin,NULL)

// For backwards compatibility
#define MAKE_MOTOR(inverted, motorPin, directionPin)  MAKE_MOTOR_2_PIN(inverted, motorPin, directionPin)


typedef MOTOR* /*PROGMEM*/  MOTOR_LIST;

typedef struct s_motor_driver{
	MOTOR_LIST* const motors;		// The motors
	uint8_t num_motors; 			// The total number of motors
} MOTOR_DRIVER;

#define MAKE_MOTOR_DRIVER(motorlst) { \
	motorlst, \
	(uint8_t)(sizeof(motorlst)/sizeof(MOTOR*)) \
}


// Use PWM output
void motorPWMInit(MOTOR_DRIVER* driver, uint32_t freq);

// Use code in Motors/L293D.c
void motorL293Init(MOTOR_DRIVER* driver, uint32_t freq);

// SN754410 is pseudonym for L293
static __inline__ void motorSN754410Init(MOTOR_DRIVER* driver, uint32_t freq){
	motorL293Init(driver, freq);
}


#endif /* MOTORPWM_H_ */
