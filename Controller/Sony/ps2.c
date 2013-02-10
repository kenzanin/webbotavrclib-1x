
/*
 * $Id: ps2.c,v 1.5 2010/06/14 18:43:18 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: ps2.c,v $
 * Revision 1.5  2010/06/14 18:43:18  clivewebster
 * Add copyright license info
 *
 * Revision 1.4  2010/06/13 15:09:55  clivewebster
 * Add rumble, button pressures and additional button test routines.
 *
 * Revision 1.3  2010/05/19 01:56:20  clivewebster
 * Increase retries when entering analogue mode
 *
 * Revision 1.2  2010/05/09 22:01:35  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/04/12 23:18:37  clivewebster
 * *** empty log message ***
 *
 * ===========
 * 
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
 *
 * ps2.c
 *
 *  Created on: 10 Apr 2010
 *      Author: Clive Webster
 */
//#define DEBUG
#include "ps2.h"
#include "../../timer.h"

#ifdef DEBUG
#include "../../rprintf.h"
#endif

// Only gives digital buttons
#define PS2_MODE_DIGITAL 		0x41
// ?
#define PS2_MODE_NEGCON  		0x23
// Gives analog joystick commands
#define PS2_MODE_ANALOGUE_RED  	0x73

// Give analog joystick commands + button pressures
#define PS2_MODE_ANALOGUE_PRESSURE 0x79
// ?
#define PS2_MODE_ANALOGUE_GREEN	0x53


// The default reply
static uint8_t PROGMEM reply[18] = {
		255,255,				// No buttons pressed
		128,128,128,128,		// Joysticks are centered
		0,0,0,0,0,0,0,0,0,0,0,0 // Pressure settings
};

// Command to enter config mode
static uint8_t PROGMEM cmd_config[] = {0x01,0x43,0x00,0x01,0x00};

// Command to set analogue mode
static uint8_t PROGMEM cmd_analog[] = {0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};

// Enable runmble
static uint8_t PROGMEM cmd_rumble[] = {0x01,0x4d,0x00,0x00,0x01,0xff,0xff,0xff,0xff};

// Command to enter DS2 config mode
static uint8_t PROGMEM cmd_enterDS2[] = {0x01,0x4f,0x00,0xff,0xff,0x03,0x00,0x00,0x00};

// Command to exit DS2 config mode
// static uint8_t PROGMEM cmd_exitDS2[] = {0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};

// Command to exit config mode
static uint8_t PROGMEM cmd_normal[] = {0x01,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


boolean sonyPS2_isAnalogMode(const SONY_PS2* controller){
	return (controller->mode == PS2_MODE_ANALOGUE_RED || controller->mode == PS2_MODE_ANALOGUE_PRESSURE) ? TRUE : FALSE;
}

// Get the bitmask of buttons that have changed state
uint16_t sonyPS2_buttonsChanged(const SONY_PS2* controller){
	return controller->previous ^ sonyPS2_buttonsRaw(controller);
}

boolean sonyPS2_read(SONY_PS2* controller){
	boolean ok = FALSE;

// inferred
//	spiDeviceSelect(controller,TRUE);					/* Select the device 			*/

//	if(spiAbstractBusGetClock(spiDeviceGetBus(controller)) < SPI_CLOCK_DIV32 ){
//		spiAbstractBusSetClock(spiDeviceGetBus(controller), SPI_CLOCK_DIV32);
//	}

	controller->previous = sonyPS2_buttonsRaw(controller);
	spiDeviceSendByte(controller,0x01);
	controller->mode  = spiDeviceXferByte(controller,0x42);
	controller->reply = spiDeviceReceiveByte(controller);

	uint8_t numb = 0;
	if(controller->reply == 0x5A ){
		if(controller->mode == PS2_MODE_ANALOGUE_PRESSURE){
			// read 2 bytes for buttons, 4 for joysticks, 12 for button pressures = 18
			numb = 18;
		}else if(controller->mode == PS2_MODE_ANALOGUE_RED){
			// read 2 bytes for buttons, 4 for joysticks = 6
			numb = 6;
		}else if(controller->mode == PS2_MODE_DIGITAL){
			// Read 2 bytes from controller for all 16 buttons
			numb = 2;
		}
		if(numb!=0){
			uint8_t i=0;
			// If rumbling then send out the rumble settings
			if(controller->rumble){
				controller->buffer[i++] = spiDeviceXferByte(controller,(controller->rumbleRight) ? 1 : 0);
				controller->buffer[i++] = spiDeviceXferByte(controller,controller->rumbleLeft);
			}

			while(i<numb){
				controller->buffer[i++] = spiDeviceReceiveByte(controller);
			}
			ok = TRUE;
		}
	}
	spiDeviceSelect(controller,FALSE);					/* Deselect the device 			*/

#ifdef DEBUG
	rprintf(" %x %x",(unsigned)(controller->mode),(unsigned)(controller->reply) );
	if(numb!=0){
		for(uint8_t i=0; i<numb; i++){
			rprintf(" %x",controller->buffer[i]);
		}
	}
	rprintf("\n");
#endif

	// Fill out the rest with the defaults
	while(numb < sizeof(controller->buffer)){
		controller->buffer[numb++] = pgm_read_byte(&reply[numb]);
	}

	return ok;
}

static void sendCmd(SONY_PS2* controller, const uint8_t* data, size_t len){
	for(int i=0; i<len; i++){
		spiDeviceSendByte(controller,pgm_read_byte(&data[i]));
	}
	spiDeviceSelect(controller,FALSE);					/* Deselect the device 			*/
	delay_ms(10);
}

boolean sonyPS2_setAnalogMode(SONY_PS2* controller){
	if(sonyPS2_read(controller)){
		int8_t retries=100;
		while(controller->mode!=PS2_MODE_ANALOGUE_PRESSURE && --retries){
			sendCmd(controller,cmd_config,sizeof(cmd_config));
			sendCmd(controller,cmd_analog,sizeof(cmd_analog));
			sendCmd(controller,cmd_normal,sizeof(cmd_normal));
			sonyPS2_read(controller);
			sonyPS2_read(controller);

			if(controller->mode==PS2_MODE_ANALOGUE_RED){
				sendCmd(controller,cmd_config,sizeof(cmd_config));
				sendCmd(controller,cmd_enterDS2,sizeof(cmd_enterDS2));
	//			sendCmd(controller,cmd_exitDS2,sizeof(cmd_exitDS2));
				sendCmd(controller,cmd_normal,sizeof(cmd_normal));

			}
			sonyPS2_read(controller);
			sonyPS2_read(controller);
		}
	}
	return sonyPS2_isAnalogMode(controller);
}

void sonyPS_setRumble(SONY_PS2* controller, uint8_t left, boolean right){
	if(left || right){
		// Turn on rumble on first use
		if(!controller->rumble){
			controller->rumble = TRUE;
			sendCmd(controller,cmd_config,sizeof(cmd_config));
			sendCmd(controller,cmd_rumble,sizeof(cmd_rumble));
			sendCmd(controller,cmd_normal,sizeof(cmd_normal));
		}
	}
	controller->rumbleRight=right;
	controller->rumbleLeft=(left) ? interpolateU(left,0,0xff,0x40,0xff) : 0;
}

boolean sonyPS2_calibrate(SONY_PS2* controller, uint8_t deadzone){
	controller->deadzone = deadzone;
	if(sonyPS2_setAnalogMode(controller)){
		controller->calibration[0] = sonyPS2_joystickRaw(controller,PS2_STICK_RIGHT_X);
		controller->calibration[1] = sonyPS2_joystickRaw(controller,PS2_STICK_RIGHT_Y);
		controller->calibration[2] = sonyPS2_joystickRaw(controller,PS2_STICK_LEFT_X);
		controller->calibration[3] = sonyPS2_joystickRaw(controller,PS2_STICK_LEFT_Y);
		return TRUE;
	}
	return FALSE;
}

int8_t sonyPS2_joystick(const SONY_PS2* controller, PS2_STICK stick){
	int16_t raw = sonyPS2_joystickRaw(controller,stick);
	if(stick < PS2_STICK_DPAD_X){
		raw -= controller->calibration[stick - PS2_STICK_RIGHT_X];
	}else{
		// DPAD virtual joystick. Make -128 to +127
		raw -= 128;
	}

	int16_t dzPlus = controller->deadzone;
	int16_t dzMinus = 0 - dzPlus;
	if(raw > dzPlus){
		raw -= dzPlus;
	}else if(raw < dzMinus){
		raw -= dzMinus;
	}else{
		raw = 0;
	}

	int8_t rtn = CLAMP(raw, -128, 127);
	return rtn;
}

static uint8_t btnJoystick(const SONY_PS2* controller,uint16_t plusBtn, uint16_t minusBtn){
	int16_t plus = sonyPS2_buttonPressure(controller,plusBtn);
	int16_t minus = sonyPS2_buttonPressure(controller,minusBtn);
	int16_t diff = plus - minus;
	diff = diff / 2;
	diff += 128;
	return (uint8_t)diff;
}

// Return value 0..255 where 128 is the center
uint8_t sonyPS2_joystickRaw(const SONY_PS2* controller, PS2_STICK stick){
	if(stick == PS2_STICK_DPAD_X){
		return btnJoystick(controller,PS2_DPAD_RIGHT,PS2_DPAD_LEFT);
	}

	if(stick == PS2_STICK_DPAD_Y){
		return btnJoystick(controller,PS2_DPAD_DOWN,PS2_DPAD_UP);
	}

	if(stick == PS2_STICK_SHAPE_X){
		return btnJoystick(controller,PS2_BTN_CIRCLE,PS2_BTN_SQUARE);
	}
	if(stick == PS2_STICK_SHAPE_Y){
		return btnJoystick(controller,PS2_BTN_X,PS2_BTN_TRIANGLE);
	}

	return controller->buffer[stick];
}


uint16_t sonyPS2_buttonsRaw(const SONY_PS2* controller){
	uint16_t rtn = get_uint16(controller->buffer,0);
	return rtn ^= 0xFFFFU;		// invert the bits
}

boolean sonyPS2_buttonPressed(const SONY_PS2* controller, uint16_t button){
	return (sonyPS2_buttonsRaw(controller) & button) ? TRUE : FALSE;
}

uint8_t sonyPS2_buttonPressure(const SONY_PS2* controller, uint16_t button){
	uint8_t rtn = (sonyPS2_buttonPressed(controller,button)) ? 255 : 0;

	if(rtn && controller->mode == PS2_MODE_ANALOGUE_PRESSURE){
		uint8_t inx = 0;
		switch(button){
		case PS2_DPAD_UP:
			inx = 8;
			break;
		case PS2_DPAD_RIGHT:
			inx = 6;
			break;
		case PS2_DPAD_DOWN:
			inx = 9;
			break;
		case PS2_DPAD_LEFT:
			inx = 7;
			break;
		case PS2_BTN_L2:
			inx = 16;
			break;
		case PS2_BTN_R2:
			inx = 17;
			break;
		case PS2_BTN_L1:
			inx = 14;
			break;
		case PS2_BTN_R1:
			inx = 15;
			break;
		case PS2_BTN_TRIANGLE:
			inx = 10;
			break;
		case PS2_BTN_CIRCLE:
			inx = 11;
			break;
		case PS2_BTN_X:
			inx = 12;
			break;
		case PS2_BTN_SQUARE:
			inx = 13;
			break;
		}
		if(inx){
			rtn = controller->buffer[inx];
		}
	}
	return rtn;
}
// Has button just been pressed?
boolean sonyPS2_buttonDown(const SONY_PS2* controller, uint16_t button){
	return(
			((controller->previous & button)==0) && 	// was not pressed
			sonyPS2_buttonPressed(controller,button)	// and is now pressed
	) ? TRUE : FALSE;

}
// Has button just been released?
boolean sonyPS2_buttonUp(const SONY_PS2* controller, uint16_t button){
	return(
			((controller->previous & button)==button) && // was pressed
			!sonyPS2_buttonPressed(controller,button)	// and is now released
	) ? TRUE : FALSE;
}

// Has button been held down?
boolean sonyPS2_buttonHeld(const SONY_PS2* controller, uint16_t button){
	return(
			((controller->previous & button)==button) && // was pressed
			sonyPS2_buttonPressed(controller,button)	// and is now pressed
	) ? TRUE : FALSE;
}

