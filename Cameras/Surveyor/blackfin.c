/*
 * $Id: blackfin.c,v 1.6 2010/06/14 18:41:37 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: blackfin.c,v $
 * Revision 1.6  2010/06/14 18:41:37  clivewebster
 * Add copyright license info
 *
 * Revision 1.5  2010/02/18 00:02:24  clivewebster
 * Changed case sensitive Blackfin.h to blackfin.h
 *
 * Revision 1.4  2009/12/27 21:53:17  clivewebster
 * *** empty log message ***
 *
 * Revision 1.3  2009/11/02 18:34:07  clivewebster
 * Added revision log
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
 *
 *
 * blackfin.c
 *
 *  Created on: 11-Aug-2009
 *      Author: Clive Webster
 */
#include "blackfin.h"

#include "../../rprintf.h"
#include "../../timer.h"
#include <stdlib.h>


// The longest command string
#define MAX_COMMAND 25
size_t _blackfin_index;
static char _blackfin_command[MAX_COMMAND+1];			// buffer to hold outgoing command

// How long to keep checking for new data
#define MAX_RETRIES 65000

static UART* _blackfin_uart;							// The currently active uart




// rprintf destination to write to the command string
MAKE_WRITER(_blackfin_putcmd){
	_blackfin_command[_blackfin_index] = byte;
	if(byte!='\0' && _blackfin_index < MAX_COMMAND){
		_blackfin_index++;
	}
	_blackfin_command[_blackfin_index] = '\0';
	return byte;
}

// Output a character to the Blackfin camera UART
MAKE_WRITER(_blackfin_putchar){
	if(_blackfin_uart != null){
		_uartSendByte(_blackfin_uart, byte);
	}
	return byte;
}

// Set the active uart for rprintf to go to the Blackfin
Writer _blackfin_set_active(UART* uart){
	_blackfin_uart = uart;
	return rprintfInit(&_blackfin_putchar);
}


int __blackfin_get_args(const BLACKFIN_CAMERA* camera, int32_t * values, size_t numValues, boolean trace){
	int received=0;
	int args = 0;
	char c;

	while(numValues>0 && received!='\n'){
		boolean negative = FALSE;

		// Blank next value
		values[args] = 0;

		// Get a character and ignore anything not a digit or newline
		while(1){
			received = __uartGetByte(camera->camera);
			if(received!=-1){
				c = received;
#ifdef BLACKFIN_DEBUG
				if(trace){
					rprintfChar(c);
				}
#endif
				if( c=='-'){
					negative = (negative==FALSE) ? TRUE : FALSE;
					continue;
				}
				if( (c>='0' && c<='9') || c=='\n'){
					break;
				}
				negative = FALSE;
			}
		}

		// Now get the number
		for(;received!='\n';){
			if(received!=-1){
				char c = received;
				if(c>='0' && c<='9'){
					values[args] *= 10;
					values[args] += (c - '0');
					// get next character
					while(1){
						received = __uartGetByte(camera->camera);
						if(received!=-1){
							char c = received;
#ifdef BLACKFIN_DEBUG
							if(trace){
								rprintfChar(c);
							}
#endif
							if(c!='\r'){
								break;
							}
						}
					}
				}else{
					if(negative){
						values[args] *= -1;
					}
					break; // no longer a digit so exit for loop
				}
			}
		}
		args++;
		numValues--;
	}
	return args;
}



// Issue a command to the blackfin
// camera - The camera
// response - The expected response
// error - The potential error msg or null if there isn't one
// values - Where to return the arguments
// numValues - The maximum number of arguments to return
// Returns the number of arguments found or -1 if there is an error

int __blackfinCommand(const BLACKFIN_CAMERA* camera, const char* response, const char* error, int32_t * values, size_t numValues){
	int received;
	uint16_t noDataCounter;
	int counter;
	int attempt=0;
	int args = 0;
	char c;
	const char* msg=null;
//	char first = pgm_read_byte(&response[0]);


	// Terminate the command string
	_blackfin_putcmd('\0');

#ifdef BLACKFIN_DEBUG
	Writer old = _blackfin_set_active(camera->debug);
#endif

	// Try resending the message
retry:
	noDataCounter=0;
	counter=0;

	//Send rprintf to the debugger
#ifdef BLACKFIN_DEBUG
	rprintf("\n=>%s\n<=",_blackfin_command);
	// Wait for transmission to end
	while(__uartIsBusy(camera->debug));
#endif

	// Start using the queue
	__uartFlushReceiveBuffer(camera->camera);

	// Send the command to the camera
	char* str = _blackfin_command;
	while (*str)
		_uartSendByte(camera->camera, *str++);
	goto listen;


getNext:
	counter++;
	if(pgm_read_byte(&msg[counter]) == '\0'){
		goto gotCmd;
	}

	// Now listen for the response
listen:
	received = __uartGetByte(camera->camera);
	if(received != -1){
		// Got a character
		noDataCounter = 0;	// we have received a byte

		// Ignore carriage returns
		if(received=='\r'){
			goto listen;
		}
#ifdef BLACKFIN_DEBUG
		// Echo to debugger
		rprintfChar(received);
#endif

		// Translate to a character
		c = (received=='\n') ? '\0' : (char)received;

		// If its not the character we expected then start from beginning again
		if(counter != 0 && c != pgm_read_byte(&msg[counter])){
			// Try to find start of message
			counter = 0;
		}

		if(counter==0){
			// Check for start of a response or the start of an error msg
			if(c == pgm_read_byte(response)){
				// Its the start of a response
				msg = response;
				goto getNext;
			}
			if(error){
				if(c == pgm_read_byte(error)){
					msg = error;
					goto getNext;
				}
			}
			// listen for first char
			goto listen;
		}

		// We have a matching character
		goto getNext;

	}else{

		// No character available

		// No data received yet - so increment loop count
		if(counter==0 && ++noDataCounter >= MAX_RETRIES){

			// Resend the command 10 timers
			if(++attempt < 10){
				goto retry;
			}

			// we aint had no response for a long time
#ifdef BLACKFIN_DEBUG
			rprintf("Timeout Expecting:");rprintfProgStr(response);rprintfCRLF();
#endif
			args=-1;
			goto exit;
		}
		goto listen;
	}

	// We have received the command
gotCmd:
	if(msg==error){
#ifdef BLACKFIN_DEBUG
		rprintf("Recognised Error:");rprintfProgStr(error);rprintfCRLF();
#endif
		args = -1;
		goto exit;
	}
	args = __blackfin_get_args(camera, values, numValues, FALSE);

exit:

#ifdef BLACKFIN_DEBUG
	// Restore rprintf to original position
	rprintfInit(old);
#endif
	return args;
}


void blackfinSetResolution(BLACKFIN_CAMERA* camera, BLACKFIN_RESOLUTION res){
	const char* response;

	// Make rprintf go to _blackfin_command
	Writer old = rprintfInit(&_blackfin_putcmd);
	_blackfin_index=0;


	// Remember the current resolution
	camera->res = res;

	switch(res){
	case BLACKFIN_1280_BY_1024:
		rprintf("A");
		response = PSTR("#A");
		break;
	case BLACKFIN_320_BY_240:
		rprintf("b");
		response = PSTR("#b");
		break;
	case BLACKFIN_640_BY_480:
		rprintf("c");
		response = PSTR("#c");
		break;
	default:
		rprintf("a");
		response = PSTR("#a");
		break;
	}

	// Send the command
	__blackfinCommand(camera,response,null,null,0);

	// Restore rprintf to original position
	rprintfInit(old);


	//must delay so that auto-brightness has time to adjust
	delay_ms(2000);
}

void blackfinInit(BLACKFIN_CAMERA* camera){

//	__uartAttach(camera->camera, &__blackfin_discard);
//	_uartInit(camera->camera, 115200);

	// allocate space for color
	if(camera->mean == null){
		camera->mean = malloc(sizeof(COLOR));
	}

	if(camera->camera->rxBuffer== null){
		setError(BLACKFIN_RX_BUFFER);
	}

	// Set the current resolution
	blackfinSetResolution(camera, camera->res);

}


void _blackfin_sendYUV(uint8_t c){
	rprintfChar( (c / 100)+'0'); c = c % 100;
	rprintfChar( (c /  10)+'0'); c = c % 10;
	rprintfChar( c +'0');

}




