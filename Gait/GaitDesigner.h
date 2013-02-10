/*
 * $Id: GaitDesigner.h,v 1.2 2010/06/14 18:38:51 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: GaitDesigner.h,v $
 * Revision 1.2  2010/06/14 18:38:51  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/04/25 03:32:26  clivewebster
 * Add Gait behaviour
 *
 * ===========
 * 
 * Copyright (C) 2010 Clive Webster (Webbot)
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
 * GaitDesigner.h
 *
 *  Created on: 21 Apr 2010
 *      Author: Clive Webster
 *
 *  Use this to design your gait in conjunction with Gait Designer on the pc
 *
 *
 */

#ifndef GAITDESIGNER_H_
#define GAITDESIGNER_H_

// Include stuff common to all actuators
#include "../actuators.h"
#include "../_uart_common.h"

// -- Define structure and constructor for a gait designer
typedef struct s_gait_desinger {
	ACTUATOR_LIST* const actuators;			// The list of actuators to control
	uint8_t 	 num_actuators;				// The number of actuators in the list
	UART* const	 uart;						// The uart used to listen to the PC
	BAUD_RATE	 baudRate;					// The baud rate to listen at
	uint8_t*	 buffer;					// The message buffer
	uint8_t		 msgInx;
} GAIT_DESIGNER;

#define MAKE_GAIT_DESIGNER(lst, uart, baud) { \
	lst, \
	(uint8_t)(sizeof(lst)/sizeof(__ACTUATOR*)), \
	&((uart)->_uart_), \
	baud, \
	null,0 \
}

// -- Define publc methods --
void gaitDesignerInit(GAIT_DESIGNER* gait);			// Initialise it
void gaitDesignerProcess(GAIT_DESIGNER* gait);		// Process incoming message


#endif /* GAITDESIGNER_H_ */
