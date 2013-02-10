
/*
 * $Id: sdCard.c,v 1.7 2011/07/06 00:16:48 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: sdCard.c,v $
 * Revision 1.7  2011/07/06 00:16:48  clivewebster
 * Add PROGMEM to class def
 *
 * Revision 1.6  2010/08/27 16:42:21  clivewebster
 * Update DEBUG info
 *
 * Revision 1.5  2010/08/24 01:24:59  clivewebster
 * Add more DEBUG messages
 *
 * Revision 1.4  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.3  2010/03/21 15:57:34  clivewebster
 * *** empty log message ***
 *
 * Revision 1.2  2010/03/19 01:50:50  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/08 03:10:08  clivewebster
 * Added
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
 * sdCard.c
 *
 *  Created on: 1 Mar 2010
 *      Author: Clive Webster
 */

//#define DEBUG_SD

#include "sdCard.h"
#include "../timer.h"

#ifdef DEBUG_SD
#include "../rprintf.h"
#endif

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/* R1 format responses (ORed together as a bit-field) */
//#define R1_NOERROR		0
//#define R1_IDLE      	1
//#define R1_ERASE     	2
//#define R1_ILLEGAL   	4
//#define R1_CRC_ERR   	8
//#define R1_ERASE_SEQ 	16
//#define R1_ADDR_ERR  	32
//#define R1_PARAM_ERR 	64

static void xmit(const SD_CARD* card, uint8_t byte){
	#ifdef DEBUG_SD
	rprintf(">0x");rprintfu08(byte);
	#endif
	spiDeviceSendByte(card,byte);
}

static uint8_t receive(const SD_CARD* card){
	uint8_t rtn = spiDeviceReceiveByte(card);
	#ifdef DEBUG_SD
	rprintf("<0x");rprintfu08(rtn);
	#endif
	return rtn;
}

static uint8_t wait_ready(SD_CARD* card){
	uint8_t res;

	TICK_COUNT timer2 = clockGetus();;
	receive(card);
	do{
		res = receive(card);			// Timeout after 500ms
	}while ((res != 0xFF) && !clockHasElapsed(timer2, 500000));

	return res;
}

static void deselect(SD_CARD* card){
	spiDeviceSelect(card, FALSE);	// deselect the card
	receive(card);
}

static boolean select(SD_CARD* card){	/* TRUE:Successful, FALSE:Timeout */
	spiDeviceSelect(card, TRUE);	// select the card
	if (wait_ready(card) != 0xFF) {
		deselect(card);
		return FALSE;
	}
	return TRUE;
}

static boolean rcvr_datablock(SD_CARD* card, uint8_t *buff, size_t btr ){
	uint8_t token;


	TICK_COUNT Timer1 = clockGetus();
	do {							/* Wait for data packet in timeout of 200ms */
		token = receive(card);
	} while ((token != 0xFE) && !clockHasElapsed(Timer1, 200000));

	spiDeviceReceiveBytes(card, buff,btr);

	spiDeviceReceiveWord(card);		/* Discard CRC */

	return TRUE;					/* Return with success */
}

static boolean xmit_datablock(SD_CARD* card, const uint8_t *buff, uint8_t token){
	if (wait_ready(card) != 0xFF){
		return FALSE;
	}

	xmit(card,token);					/* Xmit data token */
	if (token != 0xFD) {				/* Is data token */

		spiDeviceSendBytes(card, buff, 512);
		spiDeviceSendWord(card,0xFFFF); /* CRC (Dummy)		*/

		uint8_t resp = receive(card);	/* Reveive data response token */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}


static uint8_t send_cmd(SD_CARD *card,uint8_t cmd, uint32_t param){
	uint8_t res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(card,CMD55, 0);
		if (res > 1)
			return res;
	}

	#ifdef DEBUG_SD
	rprintf("\nSend cmd %d:",(cmd & 0x3f));
	#endif

	/* Select the card and wait for ready */
	deselect(card);

	if(cmd == CMD0){
		spiDeviceSelect(card, TRUE);	// select the card - with no reading of status
	}else{
		if (!select(card))
			return 0xFF;
	}

	// Calc checksum
	uint8_t n = 1;						/* dummy CRC + stop bit */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */

	uint8_t cmdBuf[] = { cmd, (param>>24),(param>>16),(param>>8),(param), n};
	spiDeviceSendBytes(card, cmdBuf, sizeof(cmdBuf));

	/* Receive command response */
	if (cmd == CMD12)
		receive(card);					/* Skip a stuff byte when stop reading */

	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do{
		res = receive(card);
	}while ((res & 0x80) && --n);

	return res;							/* Return with the response value */

}

static boolean enterIdleState(SD_CARD* card){
	for(uint16_t n=0; ; n++){
		if(n == MAX_U16){
			return FALSE;					// Timed out
		}
		if(send_cmd(card, CMD0, 0)==1){		// send command 0 - Enter idle state
			break;
		}
	}
	return TRUE;
}

static uint32_t getTotalDiskSectors(SD_CARD* card){
	uint32_t rtn = 0;
	uint8_t csd[16];
	uint16_t csize;

	if ((send_cmd(card,CMD9, 0) == 0) && rcvr_datablock(card, csd, 16)) {
		if ((csd[0] >> 6) == 1) {						/* SDC ver 2.00 */
			csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
			rtn = (uint32_t)csize << 10;
		} else {										/* SDC ver 1.XX or MMC*/
			uint8_t n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
			csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
			rtn = (uint32_t)csize << (n - 9);
		}
	}
	return rtn;
}


boolean sdCardInit(SD_CARD* card){
	boolean rtn = FALSE;

	uint8_t n;
	uint8_t cmd;
	uint8_t cardType = 0;

	#ifdef DEBUG_SD
	rprintf("\nStart: sdCard init\n");
	#endif

	for (n = 100; n; n--)
		receive(card);							/* dummy clocks */

	// Helps fix re-initing a card thats try to do something
	spiDeviceSelect(card, TRUE);				// select the card
	for(uint16_t i = 512; i; i--)
		receive(card);							/* dummy clocks */
	spiDeviceSelect(card, FALSE);				// deselect the card

	// Try to go into idle state
	#ifdef DEBUG_SD
	rprintf("\nEnter idle state\n");
	#endif
	if(!enterIdleState(card)){						// CMD0
		#ifdef DEBUG_SD
		rprintf("\nEnter idle state FAILED\n");
		#endif
		goto exit;
	}

	TICK_COUNT timer1 = clockGetus();				/* Initialization timeout of 1 second */


	// CMD8 is new to version 2
	if( (send_cmd(card,CMD8, 0x1AA)) == 1 ) {			/* SDHC */
		uint8_t ocr[4];								// remaining four bytes from R7 response
		#ifdef DEBUG_SD
		rprintf("\n**Version 2'ish**\n");
		#endif
		for (n = 0; n < 4; n++){
			ocr[n] = receive(card);					/* Get trailing return value of R7 resp */
		}
		if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
			#ifdef DEBUG_SD
			rprintf("\nLeave idle state\n");
			#endif
			while(!clockHasElapsed(timer1,1000000) && send_cmd(card, ACMD41, 1UL << 30)){
			/* Wait for leaving idle state (ACMD41 with HCS bit) */
			}

			if(!clockHasElapsed(timer1,1000000) && send_cmd(card, CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
				#ifdef DEBUG_SD
				rprintf("\nGet card type\n");
				#endif
				for (n = 0; n < 4; n++){
					ocr[n] = receive(card);
				}
				cardType = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
			}
		}else{
			#ifdef DEBUG_SD
			rprintf("\nBad voltage\n");
			#endif
		}
	} else {							/* SDSC or MMC */
		#ifdef DEBUG_SD
		rprintf("\n**Version 1'ish**\n");
		#endif
		if (send_cmd(card, ACMD41, 0) <= 1) 	{
			cardType = CT_SD1;
			cmd = ACMD41;				/* SDv1 */
		} else {
			cardType = CT_MMC;
			cmd = CMD1;					/* MMCv3 */
		}
		#ifdef DEBUG_SD
		rprintf("\nCard type:%u\n",cardType);
		rprintf("\nLeaving idle state\n");
		#endif
		while(!clockHasElapsed(timer1,1000000) && send_cmd(card, cmd, 0)){	/* Wait for leaving idle state */
		}

		if (clockHasElapsed(timer1,1000000) || send_cmd( card, CMD16, 512) != 0){	/* Set R/W block length to 512 */
			cardType = 0;
			#ifdef DEBUG_SD
			rprintf("\nTimed out or CMD16 failed\n");
			#endif
		}
	}

	if(cardType){
		card->numSectors = getTotalDiskSectors(card);
	}

exit:
	#ifdef DEBUG_SD
	rprintf("\nFinished\n");
	#endif

	deselect(card);

	if (cardType) {			/* Initialization succeded */
		rtn = TRUE;
	}

	#ifdef DEBUG_SD
	if(rtn){
		rprintf("Success: Card Type=%d\n",cardType);
	}else{
		rprintf("Failure\n");
	}
	#endif
	card->cardType = cardType;
	card->initialised=rtn;
	return rtn;


}

boolean sdCardRead(SD_CARD* card,uint32_t absSector,void* dta,uint8_t numSectors){
	if(card->initialised && numSectors!=0 && dta){
		// convert sector to address
		uint32_t address = (card->cardType & CT_BLOCK) ? absSector : absSector * 512;

		if(numSectors==1){
			/* READ_SINGLE_BLOCK */
			if ((send_cmd(card, CMD17, address) == 0)
				&& rcvr_datablock(card,dta, 512)){
				numSectors = 0;
			}
		}else{
			/* READ_MULTIPLE_BLOCKS */
			if(send_cmd(card, CMD18, address) == 0){
				do {
					if (!rcvr_datablock(card, dta, 512))
						break;
					dta += 512;
				} while (--numSectors);
				send_cmd(card, CMD12, 0);				/* STOP_TRANSMISSION */
			}
		}
		deselect(card);
	}
	return (numSectors==0) ? TRUE : FALSE;
}

boolean sdCardWrite(SD_CARD* card, uint32_t absSector,const void* dta,uint8_t numSectors){
	if(card->initialised && numSectors!=0 && dta){
		// convert sector to address
		uint32_t address = (card->cardType & CT_BLOCK) ? absSector : absSector * 512;

		if(numSectors == 1){
			/* WRITE_BLOCK */
			if ((send_cmd(card, CMD24, address) == 0)
				&& xmit_datablock(card, dta, 0xFE)){
				numSectors = 0;
			}
		}else{
			if (card->cardType & CT_SDC)
				send_cmd(card, ACMD23, numSectors);

			if (send_cmd(card, CMD25, address) == 0) {	/* WRITE_MULTIPLE_BLOCK */
				do {
					if (!xmit_datablock(card,dta, 0xFC))
						break;
					dta += 512;
				} while (--numSectors);

				if (!xmit_datablock(card, 0, 0xFD))	/* STOP_TRAN token */
					numSectors = 1;					// forced error
			}

		}
		deselect(card);
	}

	return (numSectors==0) ? TRUE : FALSE;
}

static boolean disk_read(void* device,uint32_t absSector,void* dta){
	SD_CARD* card = (SD_CARD*)device;
	return sdCardRead(card, absSector,dta,1);
}

static boolean disk_write(void* device, uint32_t absSector,const void* dta){
	SD_CARD* card = (SD_CARD*)device;
	return sdCardWrite(card, absSector,dta,1);
}

static uint32_t disk_total_sectors(void* device){
	SD_CARD* card = (SD_CARD*)device;
	return card->numSectors;
}

// Create a class with the method overrides for this type of storage class
static const STORAGE_CLASS PROGMEM c_sdCard_disk = MAKE_STORAGE_CLASS( \
		&disk_read, \
		&disk_write, \
		&disk_total_sectors);

const STORAGE_CLASS* sdCardGetStorageClass(void){
	return &c_sdCard_disk;
}

