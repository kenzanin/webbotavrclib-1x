
/*
 * $Id: AX12dumpAll.c,v 1.2 2010/06/14 19:14:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: AX12dumpAll.c,v $
 * Revision 1.2  2010/06/14 19:14:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/03/24 19:49:27  clivewebster
 * Alpha release
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
 * AX12dumpAll.c
 *
 *  Created on: 23 Mar 2010
 *      Author: Clive Webster
 */

#define RPRINTF_COMPLEX

#include "AX12.h"
#include "../../rprintf.h"
#include "../../core.h"

void ax12DumpAll(const DYNAMIXEL_AX12_DRIVER* driver){
	rprintf("AX12 ID Position Speed Load Volts Temp(C) Moving\n");
	for(int i=0; i<driver->num_servos; i++){
		DYNAMIXEL_AX12* servo = (DYNAMIXEL_AX12*)pgm_read_word(&driver->servos[i]);
		uint16_t  status = ax12GetInfo(servo);
		if(status == 0){
			rprintf("%7u %6d %6d %5d %2u.%c %5u      %c\n",
				(uint16_t)servo->id,
				servo->info.position,
				servo->info.speed,
				servo->info.load,
				(uint16_t)servo->info.voltage / 10, '0' + servo->info.voltage % 10,
				(uint16_t)servo->info.temperature,
				(servo->info.moving) ? 'Y' : 'N');

		}else{
			rprintf("%7u Status: %u\n",(uint16_t)servo->id,status);
		}
	}
}
