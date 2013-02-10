/*
 * $Id: servosConnect.c,v 1.2 2010/06/15 00:48:59 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: servosConnect.c,v $
 * Revision 1.2  2010/06/15 00:48:59  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2009/09/26 17:59:26  clivewebster
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
 * servosConnect.c
 *
 *  Created on: 26-Sep-2009
 *      Author: Clive Webster
 */
#include "servos.h"

void servosSetConnected(const SERVO_DRIVER* driver, boolean connect){
	for(int i=driver->num_servos - 1; i >= 0; i--){
		SERVO* servo = (SERVO*)pgm_read_word(&driver->servos[i]);
		act_setConnected(servo, connect);
	}
}

