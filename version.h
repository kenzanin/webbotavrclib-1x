/*
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
 * version.h
 *
 *      Author: Clive Webster
 *
 *      Version 1.0 Beta release 2 April 2009
 *
 *      Version 1.1 Beta release 8 May 2009
 *      	Fixed issuse with interrupts becoming disabled in buffer.c
 *      	Added support for software UARTs and GPS
 *
 *      Version 1.2 Beta release 2 June 2009
 *      	Fixed compile problems with serial motor drivers
 *      	Added alpha version for AxonII support via 'sys/axon2.h'
 *      	Added constant in timer.h for 'NUMBER_OF_TIMERS'
 *      	Added function in timer.h 'const IOPin* timerGetCapturePin(const Timer* timer)' to return the input capture pin
 *      		for the specified timer or null if it doesn't have one
 *      	Replaced appInit with calls to appInitHardware and appInitSoftware
 *			appControl can now return a value in us.
 *
 *		Version 1.3 Beta release 20 June 2009
 *			Fixed sabertooth issues and added choice of PACKETIZED or SIMPLE.
 *			Stopped automatic initialisation of UARTs as this caused random glitch characters to be sent
 *				at power up. Serial motor drivers still do it for you but you will now need to init any
 *				uart used by rprintf statements
 *
 *		Version 1.4
 *			Added ATMega2560 support
 *			Added 'CLAMP' to 'libdefs.h'
 *			Changed the signature of the code for rprintfInit to return the value to make it easier to do checksums etc
 *			Added SPI support in hardware (spi.h), software (spisw.h), and uart (spiUart.h)
 *			Added EEPROM support
 *
 *		Version 1.5
 *			Fixed a2d to read from multiple ports - wasn't checking for 'read complete' correctly so gave old values
 *			Fixed servos.h so that rather than calling 20ms since previous bank start it now uses 20ms since the last
 *				servo of the same bank start. So delay may well be > 20ms per servo
 *			Added example files
 *			rprintf now supports %s for printing a string from RAM
 *			rprintf.h now has void rprintfCharN(unsigned char c, size_t repeat); to print a char a number of times
 *			rprintf.h now has void rprintfMemoryDump(const void* data, size_t offset, size_t len); to dump a tabular view
 *			Added 'color.h'
 *			Added 'fraction.h' - not documented as you need to understand Fibonacci sequences. Too long to describe !! But is used
 *				by other lib functions to avoid floating point maths library.
 *
 *		Version 1.6
 *			Added I2C master mode support via i2c_master.h
 *			Changed the implementation of the delay functions, scheduler, and servos.h to give more
 *					accurate timings and less servo jitter. These changes have NO EFFECT on your own program.
 *			Added initial support for the Blackfin camera from Surveyor Corporation
 *
 *		Version 1.7
 *			Fixed issues in I2C master mode, pullups now turned on
 *			Added Motors/Toshiba/TB6612FNG.h motor driver
 *			iopin.h Added pulseIn and pulseOut
 *			sensors - restructured it all, added HMC6343 compass, ACCM3D2 accelerometer
 *			Removed original sensor.h that read a single value as some sensors can have many values.
 *			Sensors are now grouped by 'type', 'manufacturer' and 'device'.
 *			All sensors of the same 'type' will return values in the same range so that they are
 *			interchangeable. All sensors should now be initialised in your appInitHardware. These calls
 *			are of the format: <type>Init(sensor) and can be read using <type>Read(sensor) where
 *			<type> is the type of sensor eg: distance, compass, voltage etc.
 *			The <type>Read function will access the device and store the result locally. This value can
 *			be retrieved by reading: sensor.<type>.<unit>
 *			EG distance sensors return a value in cm. So the value will be in sensor.distance.cm
 *			The manual has been updated with these changes
 *
 *		Version 1.8
 *			Added a routine for each sensor type to dump its values to the rprintf destination.
 *			Added MPX5100a pressure sensor - untested
 *			Added ADXL335 accelerometer from Analog Devices - untested
 *			Fixed ACCM3D2 sensor - tested and okayed by Admin
 *			Fixed HMC6343 sensor - tested and okayed by Admin
 *			Added Gyro sensor IDG500 - untested
 *
 *		Version 1.9  1 October 2009
 *			In servos.h added servosSetConnected and servosSetSpeed to work in groups of servos
 *			Added Sensors/Gyro/InvenSense/IDG300.h
 *			Added Sensors/Gyro/ST/LPR530AL.h
 *			Added Sensors/Gyro/ST/LY530ALH.h
 *			Added support for ATMega328p (32k flash, 2k RAM, 1k EEPROM in 28 pin DIP - compatible with ATMega168)
 *			Fixed timer glitch
 *			Changed gps so that baud rate should be set by the user rather than forcing 4800 baud
 *
 *		Version 1.10 15 October 2009
 *			Accelerometers are so flakey! Use existing code to find min/max values for each axis then
 *				to make them more precise try adding
 *				accelerometerCalibrateX(device,min,max)
 *				accelerometerCalibrateY(device,min,max)
 *				accelerometerCalibrateZ(device,min,max)
 *				in your appInitHardware to force the sensor to return sensible values.
 *			Tested and fixed all sonars MaxBotix EZ1, Parallax Ping sonar, Devantech SRF-05
 *			Tested and fixed BabyOrangutan B328 board. On board motors working.
 *			Sabertooth.h moved from Motors/Dimension to Motors/DimensionEngineering
 *			Added supprt for LB1836M as used by older BabyOrangutan boards
 *			Changed Toshiba TB6612FNG to allow either 3 or 2 pin control
 *			Fixed conversions for Sharp IR distance sensors
 *			Each sys file now has a sysInitHardware to init any devices built onto the board
 *			All versions of rprintf now allow %u to print unsigned number
 *			Removed 'servoPWM.h' - its only function is now merged into 'servos.h'
 *			timer.h - added 'compareGetOutputMode' function
 *
 *		Version 1.11 27 October 2009
 *			Added pwm.h to allow you set up a hardware PWM of your choosing - see pwm.h
 *			Added interpolateU to core.h - same as 'interpolate' but copes better if the returned value
 *				is always an unsigned (positive) number
 *
 *		Version 1.12 3 November 2009
 *			Added Freescale MMA7260QT accelerometer
 *			Fixed ATmega328B UART functions
 *			Fixed Documentation example for Maxbotix EZ1
 *			Fixed some case sensitive file names for Unix
 *
 *	    Version 1.13 28 December 2009
 *	    	Fixed #includes within library for Unix (as best I can)
 *	    	Add Sensors/Distance/Devantech/SRF04_Sonar.h
 *	    	Changed blackfin camera initialisation. Old way was a bit more clumsy ie
 *	    		MAKE_BLACKFIN_CAMERA(&(UART3->__uart__)) but is now simpler ie MAKE_BLACKFIN_CAMERA(UART3);
 *			Changed blackfinFetchBlob so that the first blob# starts at 0 rather than 1
 *			Add ledseg.h to drive 8 segment leds, and also to create a marquee
 *			Change Axon2.h to set default rprintf and error log locations to go to the
 *				built in 8 segment led.
 *			Changed Axon2.h - renamed led_put_digit to led_put_char as it can now cope with more than
 *				just digits
 *		Version 1.13c 17 January 2010
 *				gps now returns track - see gps.h
 *				gpsNMEA - turned off debug mode
 *		Version 1.14 5 February 2010
 *			Changed handling of ADC pins. No longer assumes that ADC inputs are IOPins. This means that any sensors whose MAKE command
 *				specified an IOPin for an analogue input now specify an ADC_CHANNEL instead ie ADC0, ADC1, etc.
 *			Removed functions a2dConvert10bitIO and a2dConvert8bitIO as ADCs no longer have to be IOPins
 *			Added uartSendBuffer to send a sequence of bytes over the uart
 *			Added switch.h to wrap an IOPin with some more friendly macro calls for a switch.
 *			Changed Marquee to introduce a brief 'flash' so that if two chars are the same then there is a brief blink between them
 *			On Axon II removed the individual LED definitions for the segment display. They have been
 *				replaced with a single SEGLED called 'led_display'. So to turn on segment E then use
 *				segled_on(&led_display, SEGMENT_E); // turn on  segment E
 *				segled_off(&led_display,SEGMENT_E); // turn off segment E
 *			Fixed ATMega32 timer0 compare problem in device.h
 *			Added HMC6352 compass - but untested as of yet. If you use it then let me know.
 *			Added some functions to HMC6343 to allow change of reset frequency
 *			Replaced all the Sharp distance sensor header files with one file called GP2.h
 *			Added Sharp GP2Y0A02YK0F, GP2Y0A700K0F and GP2Y0A710K0F distance sensors
 *			Added Sharp GP2Y3A002K0F, GP2Y3A003K0F 'wide' sensors
 *			Moved gps.h to Sensors/GPS/gps.h
 *			Moved gpsNMEA.h to Sensors/GPS/NMEA/gpsNMEA.h
 *			MAKE_GPS_NMEA has an extra parameter for the baud rate. You no longer need to initialise
 *				the UART yourself.It just happens. This was done to reduce buffer overflows at powerup.
 *			Added alpha support for AVRcam - can't test it - don't have one and they aren't available!
 *			Stop the sys files from making pins into outputs in 'configure ports'. Its "mega-bad" as it can cause temporary short circuits and,
 *				as found/fixed earlier, can really upset UARTs.
 *		Version 1.14a 6 February 2010
 *			Fixed bug in flashing characters in the Marquee
 *			Added servoSetConfig in servos.h to allow change of servo config at runtime
 *		Version 1.15 18 February 2010
 *			Added support for ATMega2561 and ATMega1280
 *			Added GetReader for uarts and GetWriter for uarts and others outputs like Marquee, Displays etc
 *			Removed rprintMarquee as it is now obsolete
 *			Changed time for sensor 'last read' to be the time just before the sensor is read rather than
 *					the time just after the sensor is read.
 *			Added Displays folder with support for 'Sparkfun serLCD' serial displays and Matrix Orbital
 *					serial displays
 *			Changed creation of MAKE_SW_UART_BUFFERED so that the timer number is given instead of TIMERn. Saves
 *					a small amoount of RAM but mainly for Project Designer integration.
 *		Version 1.16 20 March 2010
 *			libdefs.h Added get_uint16, get_uint32, set_uint16, set_uint32
 *			timer.c  If a given pin is used as the output of both an 8 bit and a 16 bit timer then
 *					give preference to the 16 bit timer.
 *			spi*.h	The SPI has had a re-vamp and has been tested using both hardware and software bit-bang emulation
 *			Storage/sdCard.h Allows reading/writng of individual sectors or use a 'DISK' to get 
 *					a Windows readable removable hard drive.
 *			servos.h - updated for better handling. servosInit call now expects a Timer rather than
 *					a compare channel.
 *			Sensors/Temperature/Devantech/TPA81.h
 *			PID control - see pid.h
 *			HTML docs are now available at http://webbot.org.uk
 *
 *		Version 1.17 2 April 2010
 *			Support for the Dynamixel AX12 servo in Servos/Dynamixel/AX12.h
 *			Support for SPI EEPROM - in Storage/spiEEPROM.h
 *			Support for I2C EEPROM - in Storage/i2cEEPROM.h
 *			i2c_master.h - added i2cMasterSendWithPrefix and i2cMasterTransfer
 *
 *		Version 1.18 - 12 May 2010
 *			Add support for ATMega128RFA1 - but wireless connection is up to you
 *			Add support for ATMega128
 *			Add Controller/Sony/ps2.h - to connect one or more PS2 controllers
 *			Add Gait/GaitDesigner.h
 *			libdefs.h remove PI - instead use <math.h> M_PI instead
 *			
 *		Version 1.19 - 19 May 2010
 *			Added Devantech SRF02 and SRF08 distance sensors
 *			Added Devantech SD21 servo controller
 *			Added option to access Devantech CMPS03 compass via I2C
 *			Added Devantech CMS09 compass with pitch and roll via I2C
 *			Added some functions to the I2C code to make it simpler to read/write registers on the slave.
 *
 *		Version 1.20 - 7 June 2010
 *			Fixed L298 bug where motor only turned one way
 *			SPI added functions to get/set the mode, bit order, and clock
 *			SPI added generic spi device for things not directly supported
 *			Added servosCenter to servos.h 
 *			Added Text2Speech.h for speech synthesis
 *
 *		Version 1.21 - 20 June 2010
 *			Add Sensors/Encoder/Generic/fastquad.h for high resolution encoders
 *			Changed Controller/Sony/ps2.h to have rumble motors, key pressures, and some extra helper routines
 *			Added bufferBytesUsed to buffer.h
 *			Added ADXL345 accelerometer (i2c)
 *			Added HMC5843 compass (i2c)
 *			Fixed ATMega128 UART problem (darn those ATmel naming conventions!)
 *			Added Maxbotix MB7077 (works with 7067 as well) distance sonar
 *
 *		Version 1.22 - 4 July 2010
 *			iopin.h - pin_make_output now specifies the initial output value
 *			iopin.h - pin_pulseIn/Out now specifies if the pulse is active high or low
 *			HMC5842 - the compass now also returns the raw magnetometer values in rawX, rawY, and rawZ. It also calculate the
 *						roll and pitch angles
 *			Added Sensors/IMU/Sparkfun/razor.h - but needs new firmware from Admin at societyofrobots.com for the Razor
 *			Added the Maths folder for 3D vectors and matrices and 2D vectors
 *			Added GaitRunner
 *
 *		Version 1.23 - 15 July 2010
 *			Board Designer:		You an now specify the AVcc, and Vcc voltages for the processor
 *			Project Designer: 	Strip illegal characters in filenames
 *								ATMega640 showed pin 2 as RXD instead of RXD0
 *								You need to set the resolution for the Blackfin
 *								The 'File|Print' option shows IOpin numbers where relevant
 *			a2d.h - Added a2dReadMv
 *			Reworked ALL analogue sensors to work with different ADC reference voltages
 *			Added: Audio/SOMO14D audio playback device
 *			Added generic support for serial servo controller cards: see servoSerialInit
 *			Added support for the ATMega644
 *			
 *		Version 1.24 - 11 August 2010
 *			core.h - added the 'isqrt' function
 *			Added the Stepper folder for L297 and Pololu A4983 stepper motor support
 *			GaitRunner.h added gaitRunnerSetDelta and gaitRunnerGetDelta to allow the gait to be
 *				modified at runtime based on sensor data.
 *		    Changed Sensors/Encoder/Generic/fastquad.h - added MAKE_GENERIC_FAST_QUADRATUREx2
 *		        to double the encoder resolution when using two INT pins
 *
 *		Version 1.25 - 20 September 2010
 *			Add interpolation function to encoders. This means the MAKE commands have an extra parameter - so
 *			 you will need to re-edit the encoders in PD and regenerate. For non PD users then add a FALSE
 *			 parameter to the end of your MAKE to allow it to continue to function as before.
 *		    Stepper motors - added Stepper/Generic/Bipolar.h to allow use of h-bridge circuits like the
 *		    	L293 series.  And fixed some bugs in the original code.
 *
 *		Version 1.26 - 1 October 2010
 *			Replaced i2c_master.h with i2cBus.h to allow both hardware and software I2C access
 *			sd21 should now use MAKE_I2C_SERVO_DRIVER(0xC2,servo_list) the I2C address is fixed at i2c address 0xC2
 *			If using Project Designer then just regenerate your project code.
 *			servos.h added MAKE_I2C_SERVO_DRIVER
 *
 *		Version 1.27 - 9 October 2010
 *			Displays - set the back light brightness to 50% and then turn it off at startup
 *					 - SparkFun serLCD now implements the setBrightness command for the back light
 *			Motors   - Added 2pin for L293D, and compatibles, for uBotino.
 *			Encoders - speeded up the encoder_read function for when interpolation is turned off
 *			Add the tone.h file for playing single tones or ring tones
 *			Project/Board Designer - Added the uBotino board
 *								   - Changed the way the splash screen is displayed to get around bugs in some fire walls
 *
 *		Version 1.28 - 5 January 2011
 *			Fix problem with macro encoderSubtract
 *			Fix minor issues in software uart
 *			Add gyro ITG3200
 *
 *		Version 1.29 - 21 February 2011
 *			Add support for ATMega1284P
 *			Add driving up to 8 servos via one 16bit hardware PWM channel - see servos.h
 *
 *		Version 1.30 - 8 March 2011
 *			Sparfun serLED horiz/vert graph - now shows something - but low resolution!
 *			Added VT100 to allow PC to act as an LCD display (no kbd)
 *			Added Devantech LCD03 a 20/4 LCD via I2C
 *
 *		Version 1.31 - 20 April 2011
 *			The gps code has been changed to process messages in the background to reduce error 11's
 *			and gpsNMEAinit() has been added which must be called to initialise the device.
 *			You should either add this into appInitHardware or regenerate your code from Project Designer
 *			Note that the gps no longer requires a receive buffer.
 *			uartAttach now has an additional parameter which is then passed to the attached handler.
 *
 *
 *		Version 1.32 - 5 July 2011
*			I2C_DEVICE_LIST must be followed by PROGMEM
 *			ACTUATOR_LIST must be followed by PROGMEM
 *			SERVO_LIST must be followed by PROGMEM
 *			MOTOR_LIST must be followed by PROGMEM
 *			DYNAMIXEL_AX12_LIST, POLOLU_DS_MOTOR_LIST, SABERTOOTH_MOTOR_LIST, SANYO_LB1836M_MOTOR_LIST,
 *			SOLAR_L298_MOTOR_LIST, TOSHIBA_TB6612FNG_3pin_MOTOR_LIST,TOSHIBA_TB6612FNG_2pin_MOTOR_LIST,
 *			STEPPER_LIST, SEGLED_LIST, SPI_DEVICE_LIST
 *			For FAT stuff then FILE is now FATFILE
 *
 *			Fixed Project Designer for software uarts using an 8 bit timer.
 *			Fixed setting the hardware I2C bus speed to speeds below abut 70kbps.
 *
 *
 *			gpsNMEA is now a true sensor, gpsNMEAprocess return value has changed meaning
 *			gpsNMEA longitude and latitude are now returned in degrees not radians if you want to keep
 *				radians then multiply by (M_PI/180). To access the returned values then add '.gps' just before '.info'
 *				ie 'myGps.info.longitude' becomes 'myGps.gps.info.longitude'
 *			rprintf removed need to #define RPRINTF_FLOAT in order to use rprintfFloat
 *			tone.h is now in Audio/tone.h
 *			PD allow user to specify optimisation flags
 *			Fix GPS to buffer one line under interrupts but only scan it when read() is called
 *
 *		Version 1.33 10 July 2011
 *			All PWM motor drivers now allow you to set the PWM frequency
 *			Fixed an issue whereby rprintf could go wrong under interrupts
 *			DroneCellSMSInit return value MUST be checked - else compiler error
 *
 *		Version 1.34 13 August 2011
 *			Added support for HMC5883L compass (for new Razor board).
 *			Added support for BMP085 pressure sensor (with functions to return altitude)
 *			Pressure sensors now return values in Pa rather than kPa (for better accuracy)
 *			Project Designer now has all 3 versions of the Sparkfun Razor board
 *			Sharp wide angle distance sensors now return the minimum distance across all 5 sensors rather than the average
 *
 *		Version 1.35 14 October 2011
 *			Updated ADXL345 to allow you to set the refresh rate
 *			Fixed LengthSquared bug in Vector2D and Vector3D
 *			Fixed bug in HMC5883L where Y and Z registers are reversed
 *
 *		Future stuff -
 *			add a FAT file system to SPI/I2C Eeproms that are big enough
 *
 */


#ifndef VERSION_H_
#define VERSION_H_

#endif /* VERSION_H_ */
