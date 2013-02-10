/*
 * $Id: fastquadx2_0_1.c,v 1.1 2010/08/10 22:48:51 clivewebster Exp $
 * Revision History
 * ================
 * $Log: fastquadx2_0_1.c,v $
 * Revision 1.1  2010/08/10 22:48:51  clivewebster
 * Created
 *
 * ================
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
 *        File: fastquadx2_0_1.c
 *  Created on: 2 Aug 2010
 *      Author: Clive Webster
 *
 *      Channel A = INT0
 *      Channel B = INT1
 */
#include "fastquad.h"
#include "int0.h"
#include "int1.h"

#if defined(VECT_INT0) && defined(VECT_INT1)

// Channel A
#define INT_A_VECT 			VECT_INT0
#define INT_A_DDR  			DDR_INT0
#define INT_A_PORT  		PORT_INT0
#define INT_A_PIN  			PIN_INT0
#define INT_A_READ 			READ_INT0
#define INT_A_ISC			ISC_INT0
#define INT_A_ISCL			ISCL_INT0
#define INT_A_ISCH			ISCH_INT0
#define INT_A_ENABLE_PORT	ENABLE_PORT_INT0
#define INT_A_ENABLE_BIT	ENABLE_BIT_INT0

// Channel B
#define INT_B_VECT 			VECT_INT1
#define INT_B_DDR  			DDR_INT1
#define INT_B_PORT  		PORT_INT1
#define INT_B_PIN  			PIN_INT1
#define INT_B_READ 			READ_INT1
#define INT_B_ISC			ISC_INT1
#define INT_B_ISCL			ISCL_INT1
#define INT_B_ISCH			ISCH_INT1
#define INT_B_ENABLE_PORT	ENABLE_PORT_INT1
#define INT_B_ENABLE_BIT	ENABLE_BIT_INT1

#include "fastquadx2gen.h"

#else
// Not available
static void _init(SENSOR* sensor){
}
#endif

ENCODER_CLASS c_fastquadx2_INT0_INT1 = MAKE_ENCODER_CLASS(&_init,&__encoder_read,0,0);

