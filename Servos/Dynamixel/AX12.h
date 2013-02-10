/*
 * $Id: AX12.h,v 1.4 2011/07/05 23:54:31 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: AX12.h,v $
 * Revision 1.4  2011/07/05 23:54:31  clivewebster
 * DYNAMIXEL_AX12_LIST now requires PROGMEM
 *
 * Revision 1.3  2010/06/14 19:14:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/03/24 19:49:27  clivewebster
 * Alpha release
 *
 * Revision 1.1  2010/03/08 17:05:57  clivewebster
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
 * 
 *
 * AX12.h
 *
 *  Created on: 24 Feb 2010
 *      Author: Clive Webster
 *
 *  Dynamixel AX-12 Servo
 *  See: http://www.trossenrobotics.com/dynamixel-ax-12-robot-actuator.aspx
 *  Voltage: 7v to 10v recommended=9.6
 *  Current: 50mA to 0.9Amps(max)
 *  
 *  *** This is not tested as I don't have one ****
 */

#ifndef DYNAMIXEL_AX12_H_
#define DYNAMIXEL_AX12_H_

#include "../../actuators.h"
#include "../../_uart_common.h"
#include "../../iopin.h"

// Forward def
struct s_dynamixel_ax12_driver;

// The ID number to use in ax12Write when broadcasting a write to ALL servos.
#define ax12_BROADCAST_ID  0xfe

// Command values defined by the AX12 - so don't change!
#define ax12_WRITE 3		/* Write value */
#define ax12_READ  2		/* Read value */
#define ax12_REG_WRITE 4	/* Write value and wait for action*/
#define ax12_ACTION 5		/* process all REG_WRITE cmds */

// The data type used to return from ax12Info
typedef struct s_ax12_info {
	DRIVE_SPEED	position;			// current position DRIVE_SPEED_MIN to DRIVE_SPEED_MAX
	int16_t		speed;				// -0x3ff = -114 rpm  to +0x3ff = +114rpm
	int16_t	  	load;				// Present load -0x3ff to +0x3ff
	uint8_t		voltage;			// In tenths of a volt
	uint8_t		temperature;		// In celsius
	boolean		batch;				// Are we sending a command batch?
	boolean		moving;				// Is the servo moving
} AX12_INFO;


// The infomation about one servo
typedef struct s_dynamixel_ax12 {
	__ACTUATOR 			actuator;			// has all the common stuff
	struct s_dynamixel_ax12_driver* driver;	// The driver it is attached to, set by init'ing the driver
	uint8_t				id;					// unique Id 0-0xfd, 0xfe is reserved for broadcast
	boolean				continuous;			// Is it a continuous rotation (ie a motor)
	AX12_INFO			info;				// The current settings
} DYNAMIXEL_AX12;

// Define the standard constructor for a servo
#define MAKE_DYNAMIXEL_AX12(inverted, id, continuous)  { MAKE_ACTUATOR(inverted),null,id,continuous }

typedef DYNAMIXEL_AX12* /*PROGMEM*/  DYNAMIXEL_AX12_LIST;

typedef struct s_dynamixel_ax12_driver{
	DYNAMIXEL_AX12_LIST* const servos;		// The servos
	uint8_t num_servos; 			// The total number of servos
	HW_UART*   ax12uart;			// The hardware UART to use (must be h/w as baud=1,000,000
	const IOPin* directionPin;		// Pin for changing xmit/receive buffers
	uint8_t		writeCmd;			// The current write command
} DYNAMIXEL_AX12_DRIVER;

#define MAKE_DYNAMIXEL_AX12_DRIVER(servolst, uart, dirpin) { \
	servolst, \
	(uint8_t)(sizeof(servolst)/sizeof(DYNAMIXEL_AX12*)), \
	uart, \
	dirpin, \
	ax12_WRITE \
	}


// Error return codes
#define AX12_RECV_LEN 0x1000		/* we received an invalid length	*/
#define AX12_RECV_ID 0x800			/* we received an invalid servo id	*/
#define AX12_RECV_HEADER 0x400		/* we received an invalid header	*/
#define AX12_RECV_CHECKSUM 0x200	/* we received an invalid checksum	*/
#define AX12_RECV_TIMEOUT 0x100		/* we timed out when receiving  */
#define AX12_ERROR_INSTRUCTION 0x40 /* undefined instruction */
#define AX12_ERROR_OVERLOAD 0x20    /* the maximum torque cannot drive the load */
#define AX12_ERROR_CHECKSUM 0x10	/* we sent the wrong checksum to the AX12 */
#define AX12_ERROR_RANGE 0x8		/* the instruction range was invalid */
#define AX12_ERROR_OVERHEAT 0x4		/* the servo is overheating */
#define AX12_ERROR_ANGLE 0x2		/* the angle is out of range */
#define AX12_ERROR_VOLTAGE 0x1		/* the voltage is out of range */


// Define memory locations
// Single byte
#define ax12_ID						3
#define ax12_BAUD					4
#define ax12_RETURN_DELAY			5
#define ax12_TEMPERATURE_HIGH		11
#define ax12_VOLTAGE_LOW			12
#define ax12_VOLTAGE_HIGH			13
#define ax12_STATUS_RTN_LEVEL		16
#define ax12_ALARM_LED				17
#define ax12_ALARM_SHUTDOWN			18
#define ax12_TORQUE_ENABLE			24
#define ax12_LED					25
#define ax12_CW_COMPLIANCE_MARGIN	26
#define ax12_CCW_COMPLIANCE_MARGIN	27
#define ax12_CW_COMPLIANCE_SLOPE	28
#define ax12_CCW_COMPLIANCE_SLOPE	29
#define ax12_REGISTERED_INST		44
#define ax12_LOCK					47
// Double byte
#define ax12_CW_ANGLE_LIMIT			6
#define ax12_CCW_ANGLE_LIMIT		8
#define ax12_MAX_TORQUE				14
#define ax12_GOAL_POSITION			30
#define ax12_MOVING_SPEED			32
#define ax12_TORQUE_LIMIT			34
#define ax12_PUNCH					48
// Read only
#define ax12_PRESENT_POSITION		36
#define ax12_PRESENT_SPEED			38
#define ax12_PRESENT_LOAD			40
#define ax12_PRESENT_VOLTAGE		42		/* 8 bit */
#define ax12_PRESENT_TEMPERATURE	43		/* 8 bit */
#define ax12_MOVING					46		/* 8 bit */



// Single byte  commands
#define ax12SetID(servo,val)					ax12Write8(servo,ax12_ID, CLAMP(val, 0,253))
#define ax12SetBAUD(servo,val)				  	ax12Write8(servo,ax12_BAUD, CLAMP(val, 0,254))
#define ax12SetRETURN_DELAY(servo,val)		  	ax12Write8(servo,ax12_RETURN_DELAY, CLAMP(val, 0,254))
#define ax12SetTEMPERATURE_HIGH(servo,val)	  	ax12Write8(servo,ax12_TEMPERATURE_HIGH, CLAMP(val, 0,150))	/* celsius			*/
#define ax12SetVOLTAGE_LOW(servo,val)			ax12Write8(servo,ax12_VOLTAGE_LOW, CLAMP(val,50,250))			/* tenths of a volt */
#define ax12SetVOLTAGE_HIGH(servo,val)		  	ax12Write8(servo,ax12_VOLTAGE_HIGH, CLAMP(val,50,250))		/* tenths of a volt */
#define ax12SetSTATUS_RTN_LEVEL(servo,val)	  	ax12Write8(servo,ax12_STATUS_RTN_LEVEL, CLAMP(val, 0,  2))
#define ax12SetALARM_LED(servo,val)			  	ax12Write8(servo,ax12_ALARM_LED, CLAMP(val, 0,127))
#define ax12SetALARM_SHUTDOWN(servo,val)		ax12Write8(servo,ax12_ALARM_SHUTDOWN, CLAMP(val, 0,127))
#define ax12SetTORQUE_ENABLE(servo,val)		  	ax12Write8(servo,ax12_TORQUE_ENABLE, CLAMP(val, 0,  1))
#define ax12SetLED(servo,val)			  		ax12Write8(servo,ax12_LED, CLAMP(val, 0,  1))
#define ax12SetCW_COMPLIANCE_MARGIN(servo,val)  ax12Write8(servo,ax12_CW_COMPLIANCE_MARGIN, CLAMP(val, 0,254))
#define ax12SetCCW_COMPLIANCE_MARGIN(servo,val) ax12Write8(servo,ax12_CCW_COMPLIANCE_MARGIN, CLAMP(val, 0,254))
#define ax12SetCW_COMPLIANCE_SLOPE(servo,val)   ax12Write8(servo,ax12_CW_COMPLIANCE_SLOPE, CLAMP(val, 0,254))
#define ax12SetCCW_COMPLIANCE_SLOPE(servo,val)  ax12Write8(servo,ax12_CCW_COMPLIANCE_SLOPE, CLAMP(val, 0,254))
#define ax12SetREGISTERED_INST(servo,val)  	  	ax12Write8(servo,ax12_REGISTERED_INST, CLAMP(val, 0,  1))
#define ax12SetLOCK(servo)  	  			  	ax12Write8(servo,ax12_LOCK, 1)

// Double byte  commands
#define ax12SetCW_ANGLE_LIMIT(servo,val)		ax12Write16(servo,ax12_CW_ANGLE_LIMIT,CLAMP(val,0,1023))
#define ax12SetCCW_ANGLE_LIMIT(servo,val)		ax12Write16(servo,ax12_CCW_ANGLE_LIMIT,CLAMP(val,0,1023))
#define ax12SetMAX_TORQUE(servo,val)			ax12Write16(servo,ax12_MAX_TORQUE,CLAMP(val,0,1023))
#define ax12SetGOAL_POSITION(servo,val)			ax12Write16(servo,ax12_GOAL_POSITION,CLAMP(val,0,1023))
#define ax12SetMOVING_SPEED(servo,val)			ax12Write16(servo,ax12_MOVING_SPEED,CLAMP(val,0,0x7ff))
#define ax12SetTORQUE_LIMIT(servo,val)			ax12Write16(servo,ax12_TORQUE_LIMIT,CLAMP(val,0,1023))
#define ax12SetPUNCH(servo,val)					ax12Write16(servo,ax12_PUNCH,CLAMP(val,0,1023))

void ax12Init(DYNAMIXEL_AX12_DRIVER* driver, BAUD_RATE baud);							// Initialise to a given baud rate
void ax12Send(const DYNAMIXEL_AX12_DRIVER* driver, uint8_t id, size_t len, uint8_t* data);	// Send a sequence of bytes starting with 0xff,0xff and ending in checksum
void ax12Write8(const DYNAMIXEL_AX12* servo, uint8_t cmd, uint8_t data);				// Write a single byte variable
void ax12Write16(const DYNAMIXEL_AX12* servo, uint8_t cmd, uint16_t data);				// Write a double byte variable
uint16_t ax12GetInfo(DYNAMIXEL_AX12* servo);										// Get latest info
void ax12Dump(DYNAMIXEL_AX12* servo);					// calls ax12GetInfo for 1 servo and dumps to rprintf
void ax12DumpAll(const DYNAMIXEL_AX12_DRIVER* driver);	// calls ax12GetInfo for all servos and dumps to rprintf
void ax12Begin(DYNAMIXEL_AX12_DRIVER* driver);			// all cmds are queued until ax12End
void ax12End(DYNAMIXEL_AX12_DRIVER* driver);			// perform cmds issued since the ax12Begin
#endif /* DYNAMIXEL_AX12_H_ */
