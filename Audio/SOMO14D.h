/*
 * $Id: SOMO14D.h,v 1.1 2010/07/15 14:36:22 clivewebster Exp $
 * Revision History
 * ================
 * $Log: SOMO14D.h,v $
 * Revision 1.1  2010/07/15 14:36:22  clivewebster
 * Added in Version 1.23
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
 *        File: SOMO14D.h
 *  Created on: 6 Jul 2010
 *      Author: Clive Webster
 *
 *  A module that can store and playback up to 512 different sounds/tunes
 *  Manufactured by: http://www.4dsystems.com.au/prod.php?id=73
 *  Also available from Sparkfun and others
 *
 *  Requires a 2v7-3v6 Vcc supply - although you can have 2x 1N4001 diodes to drop a 5v supply down
 *  Current = 8uA when idling and 45mA when playing
 *
 *  A capacitor of 100uF to 470uF should be connected to Vcc
 *  For 5V systems connect a resistor (100 - 470 ohms) on the CLK and DATA lines
 *
 *  Files must called 0000.adc up to 0511.ad4
 *
 *  Cards up to 2gb formatted in FAT/FAT16 format
 *
 *  Connect speaker to SPK+ and SPK-
 */

#ifndef SOMO14D_H_
#define SOMO14D_H_

#include "../iopin.h"

typedef struct s_somo14d{
	const IOPin*  clk;		// The IOPin connected to the CLK pin
	const IOPin*  data;		// The IOPin connected to the DATA pin
	const IOPin*  busy;		// The optional used to detect if a sound is playing
}SOMO14D;

#define MAKE_SOMO14D(clk,data,busy) { clk,data,busy }

void somo14D_init(const SOMO14D* device);
void somo14D_play(const SOMO14D* device, uint16_t fileNum); // play file number 0 to 511
void somo14D_stop(const SOMO14D* device);
void somo14D_volume(const SOMO14D* device, uint8_t volume); // Set volume 0..7
boolean somo14D_isBusy(const SOMO14D* device);

#endif /* SOMO14D_H_ */
