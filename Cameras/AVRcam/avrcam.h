/*
 * $Id: avrcam.h,v 1.2 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: avrcam.h,v $
 * Revision 1.2  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.1  2010/02/04 22:27:20  clivewebster
 * Added
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
 * avrcam.h
 *
 *  Created on: 31-Jan-2010
 *      Author: Clive Webster
 */

#ifndef AVRCAM_H_
#define AVRCAM_H_

#include "../_camera_common.h"


// Class definition for an AVRcam
extern CAMERA_CLASS c_avrcam;


// Define an AVRcam camera
typedef struct s_avrcam {
	CAMERA _camera_;		// Include common camera data
	boolean tracking:1;		// Is tracking mode enabled?
} AVRCAM;

// Helper routine to construct a new AVRcam
#define MAKE_AVRCAM(uart) { MAKE_CAMERA(&c_avrcam,uart), \
	FALSE  /* Not tracking */	\
}

#endif /* AVRCAM_H_ */
