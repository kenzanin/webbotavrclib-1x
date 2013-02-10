/*
 * $Id: SD21.c,v 1.5 2010/10/01 13:16:26 clivewebster Exp $
 * $Log: SD21.c,v $
 * Revision 1.5  2010/10/01 13:16:26  clivewebster
 * Refactor - change i2c function names
 *
 * Revision 1.4  2010/09/30 16:57:27  clivewebster
 * Refactored for new hw or sw i2c bus
 *
 * Revision 1.3  2010/07/15 14:39:36  clivewebster
 * Reduce uart activity if servo is inverted
 *
 * Revision 1.2  2010/06/14 19:14:07  clivewebster
 * Add copyright license info
 *
 *
 * SD21.c
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
 *  Created on: 14 May 2010
 *      Author: Clive Webster
 */
#include "SD21.h"
#include "../../core.h"
//#include "../../rprintf.h"

static void setSpeed(__ACTUATOR *actuator, DRIVE_SPEED speed){
	SERVO* servo = (SERVO*)actuator;

	DRIVE_SPEED current = servo->actuator.required_speed;
	if(servo->actuator.inverted){
		current *= -1;
	}


	// Can only cope with 21 servos
	if(speed != current && servo->delay< 21){
		uint8_t reg = (servo->delay * 3) + 1;
		uint8_t msg[3];

		struct s_servo_driver* driver = servo->driver;
		const I2C_DEVICE* i2c = &(driver->i2cInfo);

		// Get the pulse length in us
		uint16_t pulse = interpolateU(speed, DRIVE_SPEED_MIN, DRIVE_SPEED_MAX, servo->center_us - servo->range_us , servo->center_us + servo->range_us);

		msg[0] = reg;						// Register
		msg[1] = (uint8_t)(pulse & 0xff); 	// lo
		msg[2] = (uint8_t)(pulse >> 8); 	// hi

		// The I2C address is always C2
		i2cMasterSend(i2c, sizeof(msg), msg);
//		rprintf("%u = %u\n",servo->delay,pulse);
	}
}


// Define the class
static const __ACTUATOR_DRIVER_CLASS c_sd21_servos = MAKE_ACTUATOR_DRIVER_CLASS(&setSpeed,null);

void sd21Init(SERVO_DRIVER* driver){
	// Make sure each servo is initialised
	for(int8_t i= driver->num_servos-1;i>=0;i--){
		SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);
		if(servo){
			// Connect the servo to this driver
			servo->actuator.class = &c_sd21_servos;
			servo->driver = driver;
			servo->top = 0;
			servo->delay = i; // The servo number in the list
			servoSetConfig(servo,servo->center_us,servo->range_us);

			act_setConnected(servo,TRUE);

			// Start off braking
			act_setSpeed(servo,DRIVE_SPEED_BRAKE);
		}
	}
}
