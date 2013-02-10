/*
 * $Id: SD21.h,v 1.4 2010/09/30 16:57:27 clivewebster Exp $
 * $Log: SD21.h,v $
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
 * SD21.h
 *
 *  Created on: 14 May 2010
 *      Author: Clive Webster
 *
 * Devantech SD21 servo controller
 *
#include "Servos/Devantech/SD21.h"

// Define two servos on the SD21 card
SERVO left  = MAKE_SD21_SERVO(FALSE, 1500, 500);
SERVO right = MAKE_SD21_SERVO(TRUE , 1500, 50);
SERVO* PROGMEM servos1[] = {&left,&right};
SERVO_DRIVER sd21 = MAKE_I2C_SERVO_DRIVER(0xC2,servos1);


void appInit(void){
	// Initialise the servo controller
	sd21Init(&sd21);
}
 *
 */

#ifndef SD21_H_
#define SD21_H_
#include "../../servos.h"

// An SD21 servo doesn't need an iopin
// Deprecated: use MAKE_REMOTE_SERVO
#define MAKE_SD21_SERVO(inverted, center, range) MAKE_REMOTE_SERVO(inverted, center, range)

// Initialise the SD21 driver
void sd21Init(SERVO_DRIVER* driver);

#endif /* SD21_H_ */
