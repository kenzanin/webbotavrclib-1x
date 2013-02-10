
/*
 * $Id: FATImpl.c,v 1.4 2011/07/06 00:18:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATImpl.c,v $
 * Revision 1.4  2011/07/06 00:18:57  clivewebster
 * Add PROGMEM to class def and rename FILE to FATFILE
 *
 * Revision 1.3  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/03/20 00:46:06  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/08 03:10:43  clivewebster
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
 * FATImpl.c
 *
 *  Created on: 28 Feb 2010
 *      Author: Clive Webster
 *
 *  Implement an embedded FAT file system
 *
 */
#include "FATImpl.h"
#include <stdlib.h>
#include "../../rprintf.h"
/*------------------------------------------------------------------------*/
/*					Define data types			  						  */
/*------------------------------------------------------------------------*/
#define DIRFIND_FILE 0
#define DIRFIND_FREE 1


#define iomgr_isReqRo(mode)  ((mode)&(BUFFER_MODE_READONLY))
#define iomgr_isReqRw(mode)  ((mode)&(BUFFER_MODE_READWRITE))

// Structure of an entry in the disk partition table - always 16 bytes long
// This is stored in the partition sector starting at offset 0x01be
// and can contain 4 partition entries
typedef struct s_partition_entry{
	uint8_t bootFlag;			// Is this the boot partition. 0 = Inactive, 0x80=Boot partition
	uint8_t startHead;
	uint16_t startSectorCylinder;
	uint8_t type;				// The type of partition
	uint8_t endHead;
	uint16_t endSectorCylinder;
	uint32_t LBA_begin;			// Sector offset to this partitions boot sector
	uint32_t numSectors;		// Total number of sectos
} PartitionEntry;


// The recognised partition types
#define PT_FAT12  0x01
#define PT_FAT16A 0x04
#define PT_FAT16  0x06   // more than 32mb
#define PT_FAT32  0x0B
#define PT_FAT32A 0x5C
#define PT_FAT16B 0x5E




/*------------------------------------------------------------------------*/
/*					Forward Definitions									  */
/*------------------------------------------------------------------------*/
static void* _ioGetSector(const DISK* disc,SECTOR absSector, uint8_t mode);
static boolean _fatInit(DISK* disc);
static CLUSTER _diskGetFirstClusterRootDir(const DISK *disk);
static CLUSTER _navGetLastCluster(const DISK* disk,CLUSTER_NAV *nav);
static boolean _partitionFlushRange(const DISK* disk,SECTOR addr_l, SECTOR addr_h);
static CLUSTER _fatGetNextClusterAddress(const DISK* disk,CLUSTER cluster);
static CLUSTER _dirFindinDir(const DISK* disk, const char* fatname,CLUSTER firstcluster, DIR_POSITION *loc, uint8_t mode);

/*------------------------------------------------------------------------*/
/*					Disc												  */
/*------------------------------------------------------------------------*/
boolean diskInit(DISK *disc, uint8_t numBuffers, const STORAGE_CLASS* class, void* device){
	boolean rtn = FALSE;

	disc->sclass = class;
	disc->device = device;
	disc->partitionType = 0;

	// Allocate memory for the buffers
	if(disc->buffers==null){
		disc->numbuf = numBuffers;
		disc->buffers = calloc(numBuffers,sizeof(BUFFER));
	}

	// Load the master boot record
	uint8_t* buf=_ioGetSector(disc, 0, BUFFER_MODE_READONLY);

	if(buf){
		// Find the active partition
		PartitionEntry* partitions = (PartitionEntry*)(&buf[0x1BE]);
		for(int p=3; p>=0; p--){
			PartitionEntry* partition = &partitions[p];
			uint8_t type = partition->type;
			if(type == PT_FAT12  || type == PT_FAT16A || type == PT_FAT16  ||  type == PT_FAT32  || type == PT_FAT32A || type == PT_FAT16B){
				// This is a recognised partition type
				disc->partitionType = type;
				disc->partitionStartSector = partition->LBA_begin;
				disc->partitionNumSectors = partition->numSectors;
				rtn = TRUE;
			}
		}
		if(!rtn){
			// No partition - so assume whole disk is FAT32
			const STORAGE_CLASS* class = disc->sclass;

			disc->partitionType = PT_FAT32;
			disc->partitionStartSector = 0;

			SECTOR_COUNT (*getTotalSectors)(void*) = (SECTOR_COUNT (*)(void*))pgm_read_word(&class->getTotalSectors);
			disc->partitionNumSectors = getTotalSectors(disc->device);
			rtn = TRUE;
		}
		// release the sector
		_ioReleaseSector(disc,buf);

		// Initialise the file system
		rtn &= _fatInit(disc);
	}
	disc->initialised = rtn;
	return rtn;
}

// Return the starting sector for a given cluster
SECTOR _diskClusterToSector(const DISK* disk,CLUSTER cluster){
	SECTOR base=
		disk->volume.reservedSectorCount+
		disk->fatSectorCount * disk->volume.numberOfFats;

	if(disk->fatType!=32)	{
		base += disk->volume.rootDirEntryCount/16;
	}

	return( base + (cluster-2) * disk->volume.sectorsPerCluster );
}

static CLUSTER _diskGetFirstClusterRootDir(const DISK *disk){
	return(disk->fatType == 32) ? disk->volume.rootDirCluster : 1;
}

// Flush the file system to disk. This must be done for removing any drives
boolean diskFlush(const DISK *disk){
	return(_partitionFlushRange(disk,0,disk->sectorCount));
}

uint32_t diskFreeSpace(const DISK* disk){
	SECTOR_COUNT free=0;
	CLUSTER cluster;
	CLUSTER next;
	uint8_t secPerClust = disk->volume.sectorsPerCluster;

	for(cluster = 0; cluster < disk->dataClusterCount; cluster++){
		next=_fatGetNextClusterAddress(disk,cluster);
		if(next == 0){
			free += secPerClust;
		}
	}

	return (free >> 1);
}


/*------------------------------------------------------------------------*/
/*					Buffer Management									  */
/*------------------------------------------------------------------------*/

static SECTOR_BUFFER* _bufferCurrentStackEntry(BUFFER* buf){
	return &buf->stack[ buf->depth ];
}

static void _bufferSetValid(BUFFER* buf){
	_bufferCurrentStackEntry(buf)->isValid = 1;
}
static boolean _bufferIsValid(BUFFER* buf){
	return(_bufferCurrentStackEntry(buf)->isValid) ? TRUE : FALSE;
}
static void _bufferSetWritable(BUFFER* buf){
	_bufferCurrentStackEntry(buf)->isWritable = 1;
}
static void _bufferSetNotWritable(BUFFER* buf){
	_bufferCurrentStackEntry(buf)->isWritable = 0;
}
static boolean _bufferIsWritable(BUFFER* buf){
	return(_bufferCurrentStackEntry(buf)->isWritable) ? TRUE : FALSE;
}

static uint8_t _bufferGetUsageCount(BUFFER* buffer){
	return _bufferCurrentStackEntry(buffer)->usage;
}
static void _bufferDecrementUsageCount(BUFFER* buffer){
	SECTOR_BUFFER* sb = _bufferCurrentStackEntry(buffer);
	if(sb->usage != 0){
		sb->usage--;
	}
}

static uint8_t _bufferGetReferenceCount(const BUFFER* buffer){
	return buffer->reference;
}

static uint8_t _bufferGetStackDepth(const BUFFER* buffer){
	return buffer->depth;
}

static void _bufferReset(BUFFER* buffer){
	SECTOR_BUFFER* current = _bufferCurrentStackEntry(buffer);
	memclr(current,sizeof(SECTOR_BUFFER));
	buffer->reference = 0;
}

static boolean _bufferPush(BUFFER* buffer){
	if(buffer->depth >= MAX_STACK_DEPTH){
		// The iteration number is invalid
		return(FALSE);
	}

	// Increment the stack position and reset the current data
	buffer->depth++;
	_bufferReset(buffer);

	return(TRUE);
}

static boolean _bufferPop(BUFFER* buffer){
	// Make sure there is something on the stack
	if(buffer->depth==0 || buffer->depth>MAX_STACK_DEPTH){
		return(FALSE);
	}

	// Decrement the stack pointer
	buffer->depth--;

	return(TRUE);
}

static BUFFER* _bufferFindUnused(const DISK* disc){
	uint8_t bufferNum;
	BUFFER* buffer;
	for(bufferNum=0,buffer=disc->buffers; bufferNum<disc->numbuf; bufferNum++,buffer++){
		if(!_bufferIsValid(buffer)){
			return(buffer);
		}
	}
	return(null);
}


/*------------------------------------------------------------------------*/
/*					I/O Management									  	  */
/*------------------------------------------------------------------------*/

// The basic routine to call the hardware and read a sector
static boolean _ioReadSector(const DISK *disc,SECTOR absSector,void* dta){
	const STORAGE_CLASS* class = disc->sclass;

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("[r%lu]",absSector);
		rprintfInit(old);}
#endif


	boolean (*read)(void*,SECTOR,void*) = (boolean (*)(void*,SECTOR,void*))pgm_read_word(&class->read);

	return read(disc->device,absSector,dta);

}

// The basic routine to call the hardware and write a sector
// return TRUE if ok
static boolean _ioWriteSector(const DISK *disc,SECTOR absSector,const void* dta){
	const STORAGE_CLASS* class = disc->sclass;

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("[w%lu]",absSector);
		rprintfInit(old);}
#endif

	boolean (*write)(void*,SECTOR,const void*) = (boolean (*)(void*,SECTOR,const void*))pgm_read_word(&class->write);

	return write(disc->device,absSector,dta);

}


static BUFFER* _ioGetBuffer(const DISK* disc, const void* buf){
	uint8_t i;
	BUFFER* buffer;
	for(i=0,buffer=disc->buffers; i<disc->numbuf; i++,buffer++){
		if(buf == buffer->dta){
			return buffer;
		}
	}
	return(null);
}

static BUFFER* _ioFindSectorInCache(const DISK *disc, SECTOR absSector){
	uint8_t bufferNum;
	BUFFER* buffer;

	for( bufferNum=0,buffer=disc->buffers; bufferNum<disc->numbuf; bufferNum++,buffer++){
		const SECTOR_BUFFER * secbuf = _bufferCurrentStackEntry(buffer);
		if(secbuf->isValid && secbuf->sector==absSector){
			return buffer;
		}
	}
	return(null);
}

// Write sector to physical device and mark as non-writeable
// if its no longer used
static boolean _ioFlushSector(const DISK *disc, BUFFER* buffer){

	// Make sure the buffer is writeable
	if(!_bufferIsWritable(buffer)){
		// Trying to write readonly data
		return(FALSE);
	}

	// Write the sector to the physical device
	if(!(_ioWriteSector(disc,_bufferCurrentStackEntry(buffer)->sector,buffer->dta))){
		// Write error
		return(FALSE);
	}

	// If its no longer used then no need to write it later
	if(_bufferGetUsageCount(buffer) == 0){
		_bufferSetNotWritable(buffer);
	}
	return(TRUE);
}

static boolean _ioPutSectorInCache(const DISK* disc, BUFFER* buffer, SECTOR absSector){

	// Read the data from the physical device
	if(!(_ioReadSector(disc,absSector,buffer->dta))){
		return(FALSE);
	}

	// Indicate the buffer is 'in-use'
	_bufferSetValid(buffer);

	// And remember its sector address
	_bufferCurrentStackEntry(buffer)->sector = absSector;

	return(TRUE);
}


// Find the least recently used buffer - giving preference to any read only entries
static BUFFER* _ioFindLeastRecentlyUsedBuffer(const DISK* disc){
	BUFFER* rtn=null;
	boolean foundAReadOnly=FALSE;
	uint8_t refCount=MAX_U08;
	BUFFER* buffer;
	uint8_t bufferNum;

	for(bufferNum=0, buffer=disc->buffers;bufferNum<disc->numbuf;bufferNum++,buffer++){
		if(_bufferGetUsageCount(buffer)==0){
			// Nothing uses the data in this buffer

			if(!_bufferIsWritable(buffer)){
				// This is a readonly buffer
				if(!foundAReadOnly || _bufferGetReferenceCount(buffer)<=refCount ){
					// This is the best so far
					foundAReadOnly=TRUE;
					rtn=buffer;
					refCount=_bufferGetReferenceCount(buffer);
				}
			}else{
				// This is a writable buffer
				if(!foundAReadOnly && _bufferGetReferenceCount(buffer)<=refCount){
					// This is the best writable buffer
					rtn=buffer;
					refCount=_bufferGetReferenceCount(buffer);
				}
			}
		}
	}
	return(rtn);
}

static BUFFER* _ioFindStackableBuffer(const DISK* disc){
	uint8_t leastPoint=MAX_U08;
	uint8_t bufferNum;
	BUFFER* rtn=null;
	BUFFER* buffer;

	for(bufferNum=0,buffer=disc->buffers; bufferNum<disc->numbuf; bufferNum++,buffer++){
		uint8_t stackDepth = _bufferGetStackDepth(buffer);
		if( stackDepth < MAX_STACK_DEPTH){
			// Calc the score for this buffer
			uint8_t points = 0;

			// Give it a 50% score if writable so that read only
			// buffers will always be selected in preference
			if(_bufferIsWritable(buffer)){
				points+=0x7F;
			}

			// Give it 0% to 30% depending on how many stack entries there are
			// This will cause 'stack full' entries to get a worse score than others
			points += ((uint16_t)(stackDepth*0x4D))/(MAX_STACK_DEPTH);

			// Give it 0% to 20% depending on how many time it has been read from
			points += ((uint16_t)(_bufferGetReferenceCount(buffer)*0x33))/0xFF;

			// Keep the smallest score
			if(points<leastPoint){
				leastPoint=points;
				rtn=buffer;
			}
		}
	}
	return(rtn);
}


// Read an absolute sector into the cache, if not already there,
// and return its address.
// Each call should have its own matching releaseSector command
// disc 		- The disc
// absSector	- The absolute sector on the entire disc
// mode			- The mode in which the sector will be used
static void* _ioGetSector(const DISK* disc,SECTOR absSector, uint8_t mode){
	BUFFER* buffer;

	// Find the buffer containing the sector
	buffer = _ioFindSectorInCache(disc,absSector);

	if(buffer){
		// Its already in memory
		goto inRAM;
	}

	// Find an unused buffer or flush the most recently used
	buffer = _bufferFindUnused(disc);
	if(!buffer){
		// All buffers are in use

		// Find the least recently used buffer which has no references to it
		buffer = _ioFindLeastRecentlyUsedBuffer(disc);

		if(buffer && _bufferIsWritable(buffer)){
			// Write it to the physical device
			_ioFlushSector(disc,buffer);
		}
	}

	if(buffer){
		// Mark buffer as unused
		_bufferReset(buffer);
		goto readIt;
	}

	// All buffers are currently used and referenced by code
	buffer=_ioFindStackableBuffer(disc);
	if(buffer){
		if(_bufferIsWritable(buffer)){
			_ioFlushSector(disc,buffer);
		}
		if(!_bufferPush(buffer)){
			return null;
		}
readIt:
		// Read from physical device into buffer
		if(! _ioPutSectorInCache(disc, buffer,absSector) ){
			return null;
		}
inRAM:
		// Mark as writable if required
		if(iomgr_isReqRw(mode)){
			_bufferSetWritable(buffer);
		}

		// Increment usage count
		SECTOR_BUFFER* sb = _bufferCurrentStackEntry(buffer);
		if(++(sb->usage) == 0){
			(sb->usage)--;
		}


		// increment ref count
		if(++(buffer->reference) == 0){
			(buffer->reference)--;
		}


		// Return address of sector in RAM
		return(buffer->dta);
	}

	// No memory available
	return null;
}

// Indicate that a given sector buffer is no longer required
void _ioReleaseSector(const DISK* disc,const void* buf){
	// Convert to a buffer number
	BUFFER* buffer = _ioGetBuffer(disc,buf);

	// Decrement the usage count
	_bufferDecrementUsageCount(buffer);

	if(_bufferGetUsageCount(buffer)==0 && buffer->depth!=0){
		// buffer is no longer used - so restore previous

		// Write sector to physical device
		if(_bufferIsWritable(buffer)){
			_ioFlushSector(disc,buffer);
		}

		// pop data from the stack
		_bufferPop(buffer);

		// re-read the data for the buffer from the physical device
		_ioPutSectorInCache(disc, buffer, _bufferCurrentStackEntry(buffer)->sector);
	}
}

// Read a whole sector either from the cache or directly into user memory
// Return TRUE if done or FALSE if error
static boolean _ioDirectSectorRead(const DISK *disk,SECTOR address, void* buf){
	BUFFER* buffer;

	// See if its already in the cache
	buffer=_ioFindSectorInCache(disk,address);
	if(!buffer){
		// Try to find an unused cache area
		buffer=_bufferFindUnused(disk);
		if(buffer){
			// Read the sector into the cache
			if(!_ioPutSectorInCache(disk,buffer,address)){
				return(FALSE);
			}
		}
	}

	if(buffer){
		// If its now in the cache then copy out the data
		memcpy(buf,buffer->dta,512);
	}else{
		// Read it straight into user memory
		if(!_ioReadSector(disk,address,buf)){
			return(FALSE);
		}
	}

	return(TRUE);
}

static boolean _ioDirectSectorWrite(const DISK* disk,SECTOR address, const void* buf){
	BUFFER* buffer;

	// If it exists in the cache
	buffer=_ioFindSectorInCache(disk,address);
	if(buffer){
		// copy it to the cache
		memcpy(buffer->dta,buf,512);
		// Mark as needing to be written
		_bufferSetWritable(buffer);
		return(TRUE);
	}

	// Try to find an available cache buffer entry
	buffer=_bufferFindUnused(disk);
	if(buffer){
		// copy it into the cache
		memcpy(buffer->dta,buf,512);
		SECTOR_BUFFER* current = _bufferCurrentStackEntry(buffer);
		// Set cache entry to indicate it needs writing
		_bufferReset(buffer);
		current->sector = address;
		_bufferSetWritable(buffer);
		_bufferSetValid(buffer);				// indicate buffer is in use
		return(TRUE);
	}

	// Cache is full so write the sector directly from user ram to the physical device
	return _ioWriteSector(disk,address,buf);
}

static boolean _ioFlushRange(const DISK* disk,SECTOR address_low, SECTOR address_high){
	boolean rtn = TRUE;

	// Swap the high/low values if needed
	if(address_low>address_high){
		SECTOR swap=address_low;
		address_low=address_high;
		address_high=swap;
	}


	BUFFER* buffer = disk->buffers;
	for( uint8_t b = 0; b<disk->numbuf; b++,buffer++){
		SECTOR_BUFFER* current = _bufferCurrentStackEntry(buffer);
		if((current->sector >= address_low) && (current->sector <= address_high) && (_bufferIsWritable(buffer))){
			// Flush to the physical device
			rtn &= _ioFlushSector(disk,buffer);
		}
	}
	return rtn;
}

/*------------------------------------------------------------------------*/
/*					Partition Management								  */
/*------------------------------------------------------------------------*/

// Read a sector from inside a partition
void* _partitionGetSector(const DISK* disk, SECTOR partSector, uint8_t mode){
	return(_ioGetSector(disk,partSector + disk->partitionStartSector,mode));
}

boolean _partitionDirectSectorRead(const DISK *disk,SECTOR address, void* buf){
	return(_ioDirectSectorRead(disk,address + disk->partitionStartSector,buf));
}
boolean _partitionDirectSectorWrite(const DISK *disk,SECTOR address, const void* buf){
	return(_ioDirectSectorWrite(disk,address + disk->partitionStartSector,buf));
}

static boolean _partitionFlushRange(const DISK* disk,SECTOR addr_l, SECTOR addr_h){
	return( _ioFlushRange(disk,addr_l+ disk->partitionStartSector,addr_h+ disk->partitionStartSector));
}

// Clear the contents of a sector
void _partitionClearCluster(const DISK* disk,CLUSTER cluster){
	SECTOR sector = _diskClusterToSector(disk,cluster);
	for(uint8_t c=0; c < (disk->volume.sectorsPerCluster); c++,sector++){
		void* buf = _partitionGetSector(disk,sector,BUFFER_MODE_READWRITE);
		memclr(buf,512);
		partition_releaseSector(disk,buf);
	}
}


/*------------------------------------------------------------------------*/
/*					FAT Table Management								  */
/*------------------------------------------------------------------------*/

// Find the sector inside the FAT table which holds the given cluster
// Return value: Sector, or 0.
static SECTOR _fatGetSectorAddressFatEntry(const DISK *disk,CLUSTER cluster_addr){
	SECTOR_COUNT res;

	switch(disk->fatType){
		case 12:
			res=(cluster_addr * 3 / 1024);
			break;
		case 16:
			res=cluster_addr / 256;
			break;
		case 32:
			res=cluster_addr / 128;
			break;
		default:
			return 0;
	}
	if(res >= disk->fatSectorCount){
		return(0);
	}
	return(disk->volume.reservedSectorCount + res);
}

// Look in a FAT table sector to find the next cluster in the chain
static CLUSTER _fatGetNextClusterAddressInRAM(const DISK *disk,CLUSTER cluster_addr, uint8_t* buf){
	uint16_t offset;
	CLUSTER nextcluster=0;
	uint8_t hb,lb;

	switch(disk->fatType){
		case 12:
			offset = ((cluster_addr % 1024) * 3 / 2) % 512;
			hb = buf[offset];
			if(offset == 511){
				// Next byte is in the next sector
				uint8_t* buf2=_partitionGetSector(disk,_fatGetSectorAddressFatEntry(disk,cluster_addr)+1,BUFFER_MODE_READONLY);
				lb = buf2[0];
				partition_releaseSector(disk,buf2);
			}else{
				lb = buf[offset + 1];
			}
			if(cluster_addr % 2 == 0){
				nextcluster = ( ((lb&0x0F)<<8) + (hb) );
			}else{
				nextcluster = ( (lb<<4) + (hb>>4) );
			}
			break;
		case 16:
			offset=cluster_addr % 256;
			nextcluster = *((uint16_t*)buf + offset);
			break;
		case 32:
			offset=cluster_addr % 128;
			nextcluster = *((uint32_t*)buf + offset);
			break;
	}
	return(nextcluster);
}

// Given a cluster - find the next cluster in the chain
static CLUSTER _fatGetNextClusterAddress(const DISK* disk,CLUSTER cluster){
	uint32_t nextcluster=0;

	SECTOR sector=_fatGetSectorAddressFatEntry(disk,cluster);
	if( (disk->fatSectorCount > (sector - disk->volume.reservedSectorCount)) && sector!=0 ){
		void* buf=_partitionGetSector(disk,sector,BUFFER_MODE_READONLY);
		nextcluster = _fatGetNextClusterAddressInRAM(disk, cluster, buf);
		partition_releaseSector(disk,buf);
	}
	return(nextcluster);
}


// Return true if the cluster number represents the end of a cluster chain
static boolean _fatIsEndMarker(const DISK* disk, CLUSTER cluster){
	switch(disk->fatType){
		case 12:
			if(cluster < 0xFF8){
				return(FALSE);
			}
			break;
		case 16:
			if(cluster < 0xFFF8){
				return(FALSE);
			}
			break;
		case 32:
			if((cluster & 0x0FFFFFFF) < 0xFFFFFF8){
				return(FALSE);
			}
			break;
	}
	return(TRUE);
}

// Return the cluster number that represent the end of chain marker

CLUSTER _fatGiveEndMarker(const DISK* disk){
	switch(disk->fatType){
		case 12:
			return (CLUSTER)0xFFFUL;
		case 16:
			return(CLUSTER)0xFFFFUL;
		case 32:
			return(CLUSTER)0x0FFFFFFFUL;
	}
	return(CLUSTER)0UL;
}


// Scan one sector of the FAT table to find the number of contiguous clusters
// Return value: TRUE on success, or FALSE when end of chain has been hit in which
//				case the chain has the LastCluster field set
static boolean _fatGetNextClusterChain(const DISK *disk, CLUSTER_NAV *chain){

	if(chain->currentCluster==0){
		return(FALSE);
	}

	SECTOR sect=_fatGetSectorAddressFatEntry(disk,chain->currentCluster);

	// Read the FAT
	void* buf=_partitionGetSector(disk,sect,BUFFER_MODE_READONLY);


	CLUSTER dc=_fatGetNextClusterAddressInRAM(disk,chain->currentCluster,buf);
	if(_fatIsEndMarker(disk,dc)){							// If its the end of the file?
		chain->endCluster=chain->currentCluster;			// Set the last cluster
		partition_releaseSector(disk,buf);					// Release the FAT
		return(FALSE);										// Return END
	}

	chain->currentCluster=dc;								// Move to next cluster
	chain->relativeCluster++;								// We have moved forward one

	CLUSTER last=chain->currentCluster-1;					// Set up variables to ..
	CLUSTER next=last+1;									// allow the while to succeed
	CLUSTER_COUNT contiguous=0;
	while(next-1==last &&									// While we are in the same FAT sector
		  _fatGetSectorAddressFatEntry(disk,next)==sect){

		last=next;											// last cluster = next cluster
		next=_fatGetNextClusterAddressInRAM(disk,last,buf); // find next cluster
		contiguous++;										// One more cluster in the chain
	}

	chain->contiguousCount= (contiguous==0) ? 0 : contiguous-1;// Set the number of clusters in chain

	partition_releaseSector(disk,buf);						// Release the FAT

#ifdef FAT_DEBUG
		{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("NextChain: Rel=%lu, Actual=%lu, Contig=%lu\n",chain->relativeCluster,chain->currentCluster,chain->contiguousCount);
		rprintfInit(old);}
#endif


	return(TRUE);
}

// Attempt to find the given relative cluster starting at the current CLUSTER_NAV entry
// Return value: TRUE on success and FALSE if its not part of the cluster chain
boolean _fatNavigateTo(const DISK *disk, CLUSTER_NAV *nav,CLUSTER_COUNT relativeCluster){

	// If we've gone backwards then start from the beginning
	if(relativeCluster<nav->relativeCluster || nav->currentCluster==0){
#ifdef FAT_DEBUG
		{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Nav From start\n");
		rprintfInit(old);}
#endif

		nav->relativeCluster=0;
		nav->currentCluster=nav->startCluster;
		nav->contiguousCount=0;
	}

	if(nav->relativeCluster==relativeCluster){
		return TRUE;
	}

	while(nav->relativeCluster!=relativeCluster){

#ifdef FAT_DEBUG
		{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Nav find %lu, seek %lu, @%lu\n",nav->relativeCluster,relativeCluster,nav->currentCluster);
		rprintfInit(old);}
#endif

		if(nav->contiguousCount!=0){
			// We already know the next one is contiguous
			nav->contiguousCount--;
			nav->relativeCluster++;
			nav->currentCluster++;
		}else{
			// Get the next cluster chain
			if( !_fatGetNextClusterChain(disk,nav)){
				// We've hit the end
#ifdef FAT_DEBUG
		{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Nav end %lu, seek %lu, @%lu\n",nav->relativeCluster,relativeCluster,nav->currentCluster);
		rprintfInit(old);}
#endif
				return(FALSE);
			}
		}
	}
#ifdef FAT_DEBUG
		{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Nav @%lu => %lu\n",nav->relativeCluster,nav->currentCluster);
		rprintfInit(old);}
#endif
	return(TRUE);
}
// Update the FAT to indicate that cluster_addr is followed by next_cluster_addr
void _fatSetNextClusterAddress(const DISK* disk,CLUSTER cluster_addr,CLUSTER next_cluster_addr){
	SECTOR sector=_fatGetSectorAddressFatEntry(disk,cluster_addr);

	if(( disk->fatSectorCount <= (sector - disk->volume.reservedSectorCount )||(sector==0))){
	    return;
	}

	uint8_t* buf=_partitionGetSector(disk,sector,BUFFER_MODE_READWRITE);
	size_t offset;

	switch(disk->fatType){
		case 12:
			offset = ((cluster_addr%1024)*3/2)%512;
			if(offset == 511){
				if(cluster_addr%2==0){
					buf[offset] = next_cluster_addr & 0xFF;
				}else{
					buf[offset] = (buf[offset]&0xF)+((next_cluster_addr<<4)&0xF0);
				}
				// Read second sector
				uint8_t* buf2=_partitionGetSector(disk,_fatGetSectorAddressFatEntry(disk,cluster_addr)+1,BUFFER_MODE_READWRITE);
				if(cluster_addr%2==0){
					buf2[0]=(buf2[0]&0xF0)+((next_cluster_addr>>8)&0xF);
				}else{
					buf2[0]=(next_cluster_addr>>4)&0xFF;
				}
				// Write second sector
				partition_releaseSector(disk,buf2);
			}else{
				if(cluster_addr%2==0){
					buf[offset]=next_cluster_addr&0xFF;
					buf[offset+1]=(buf[offset+1]&0xF0)+((next_cluster_addr>>8)&0xF);
				}else{
					buf[offset]=(buf[offset]&0xF)+((next_cluster_addr<<4)&0xF0);
					buf[offset+1]=(next_cluster_addr>>4)&0xFF;
				}
			}
			break;
		case 16:
			offset=cluster_addr%256;
			*((uint16_t*)buf+offset)=next_cluster_addr;
			break;
		case 32:
			offset=cluster_addr%128;
			*((uint32_t*)buf+offset)=next_cluster_addr;
			break;
	}
	partition_releaseSector(disk,buf);
}

// Find a free cluster on the disk
// Return the cluster or 0 if the disk is full
CLUSTER _fatGetFreeCluster(DISK* disk){
	CLUSTER start = disk->freeClusterHint;
	if(start < 2 || start > disk->dataClusterCount){
		start = 2;
	}

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Start Get Free Cluster\n");
		rprintfInit(old);}
#endif


	SECTOR fatSector=0;
	void*  fatBuf=null;
	CLUSTER current = start;
	CLUSTER rtn = 0;
	while(rtn==0){
		SECTOR sector=_fatGetSectorAddressFatEntry(disk,current);

		if(sector != fatSector ){
			// Release any existing sector
			if(fatBuf){
				partition_releaseSector(disk,fatBuf);
				fatBuf = null;
			}
			// Read the new sector
			fatSector = sector;
			fatBuf = _partitionGetSector(disk,fatSector,BUFFER_MODE_READONLY);
		}

		CLUSTER next = _fatGetNextClusterAddressInRAM(disk,current,fatBuf);

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Cluster %lu, next=%lu\n",current,next);
		rprintfInit(old);}
#endif

		if( next == 0){
			rtn = current;
		}else{
			// try the next one and wrap around if reqd
			if(++current > disk->dataClusterCount){
				current = 2;
			}

			// If we are back at the start then exit - everthing is full
			if(current==start){
				break;
			}
		}
	}

	// Release the current buffer
	if(fatBuf){
		partition_releaseSector(disk,fatBuf);
	}
	disk->freeClusterHint = rtn;

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("End Get Free Cluster=%lu\n",rtn);
		rprintfInit(old);}
#endif

	return rtn;
}

// Append 'num_clusters' to the end of the cluster chain
// Return TRUE on success, FALSE if the FAT is full
boolean _fatExtend(DISK* disk,CLUSTER_NAV *nav,CLUSTER_COUNT num_clusters){
	CLUSTER numRemaining=num_clusters;

	// Check we have a valid start cluster
	if(nav->startCluster<2){
		return(FALSE);
	}

	CLUSTER lastCluster=_navGetLastCluster(disk,nav);
	disk->freeClusterHint = lastCluster;


	while(numRemaining > 0){

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("\nStart extend: First Cl=%lu, End Cl=%lu\n",nav->startCluster,lastCluster);
		rprintfInit(old);}
#endif

		// Find the next free cluster
		CLUSTER currentCluster = _fatGetFreeCluster(disk);

		if(currentCluster==0){
			// The disk is full
			_fatSetNextClusterAddress(disk,lastCluster,_fatGiveEndMarker(disk));
			nav->endCluster=lastCluster;
			// Return TRUE if we have added at least one cluster
			return(num_clusters == numRemaining) ? FALSE : TRUE;
		}

		// This cluster is available - so link it in
		_fatSetNextClusterAddress(disk,lastCluster,currentCluster);

		// One less to do
		numRemaining--;
		lastCluster=currentCluster;

		// If all done then mark end of chain
		if(numRemaining==0){
			_fatSetNextClusterAddress(disk,lastCluster,_fatGiveEndMarker(disk));
			nav->endCluster=lastCluster;
		}
	}

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("End extend: First Cl=%lu, End Cl=%lu, Current=%lu\n",nav->startCluster,lastCluster,nav->currentCluster);
		rprintfInit(old);}
#endif

	if(nav->totalClusters){
		nav->totalClusters+=num_clusters;
	}
	return(TRUE);
}

static boolean _fatInit(DISK* disk){

	void* buf=_partitionGetSector(disk,0,BUFFER_MODE_READONLY); /* Load Volume label */

	// Load the volume info
	disk->volume.bytesPerSector=get_uint16(buf,0x0B);
	disk->volume.sectorsPerCluster=*((char*)(buf+0x0D));
	disk->volume.reservedSectorCount=get_uint16(buf,0x0E);
	disk->volume.numberOfFats=*((char*)(buf+0x10));
	disk->volume.rootDirEntryCount=get_uint16(buf,0x11);
	disk->volume.rootDirCluster=get_uint32(buf,0x2C);
	uint16_t FatSectorCount16=get_uint16(buf,0x16);
	uint32_t FatSectorCount32=get_uint32(buf,0x24);
	uint16_t SectorCount16=get_uint16(buf,0x13);
	uint32_t SectorCount32=get_uint32(buf,0x20);

	partition_releaseSector(disk,buf);

	// Check we have a valid FAT marker
	if( get_uint16(buf,0x1FE) != 0xAA55 ){
		return FALSE;
	}


	/* Can only handle 512 byte sectors */
	if(disk->volume.bytesPerSector!=512)
		return FALSE;

	/* Sectors per cluster must be a power of 2 */
	if(!((disk->volume.sectorsPerCluster == 1 ) |
	     (disk->volume.sectorsPerCluster == 2 ) |
	     (disk->volume.sectorsPerCluster == 4 ) |
	     (disk->volume.sectorsPerCluster == 8 ) |
	     (disk->volume.sectorsPerCluster == 16) |
	     (disk->volume.sectorsPerCluster == 32) |
	     (disk->volume.sectorsPerCluster == 64) ))
		return FALSE;

	/* There should be at least 1 reserved sector */
	if(disk->volume.reservedSectorCount==0)
		return FALSE;

	/* Find the number of sectors per FAT */
	disk->fatSectorCount = (FatSectorCount16 != 0) ? FatSectorCount16 : FatSectorCount32;
	if(disk->fatSectorCount > disk->partitionNumSectors){
		return FALSE;
	}

	// Get the sector count
	disk->sectorCount = (SectorCount16!=0) ? SectorCount16 : SectorCount32;

	SECTOR rootDirSectors=((disk->volume.rootDirEntryCount*32) +
				 (disk->volume.bytesPerSector - 1)) /
				 disk->volume.bytesPerSector;
	SECTOR dataSectorCount = disk->sectorCount - (
			disk->volume.reservedSectorCount +
			(disk->volume.numberOfFats * disk->fatSectorCount) +
			rootDirSectors);

	disk->dataClusterCount = dataSectorCount / disk->volume.sectorsPerCluster;

	// Calculate the type of FAT
	if(disk->dataClusterCount < 4085){			// 2^12 - 11
		disk->fatType=12;
		disk->volume.rootDirCluster=0;
	}else if(disk->dataClusterCount < 65525){	// 2^16 - 11
		disk->fatType=16;
		disk->volume.rootDirCluster=0;
	}else{
		disk->fatType=32;
	}

	// Find the first sector for the root directory
	disk->firstSectorRootDir = disk->volume.reservedSectorCount +
	                         (disk->volume.numberOfFats * disk->fatSectorCount);
	if(disk->fatType==32){
		disk->firstSectorRootDir += (disk->volume.rootDirCluster-2) * disk->volume.sectorsPerCluster;
	}

	// Initialise the current directory as root
	disk->firstClusterCurrentDir = _diskGetFirstClusterRootDir(disk);

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("FAT@%u len=%lu\n",disk->volume.reservedSectorCount,disk->fatSectorCount);
		rprintf("Root@%lu\n",disk->firstSectorRootDir);
		rprintf("Sec/Cluster=%u\n",disk->volume.sectorsPerCluster);
		rprintfInit(old);}
#endif

	return(TRUE);
}


/*------------------------------------------------------------------------*/
/*					File Management				  						  */
/*------------------------------------------------------------------------*/

// Convert a character into a valid uppercase character for a filename
// Invalid characters are changed to an '_'
static char _fileValidateChar(char c){
    // Convert to uppercase
    if( c>='a' && c<='z' )
		return (c-'a'+'A');

    if(c=='-' || c=='_' || c=='~' || (c>='0' && c<='9') || (c>='A' && c<='Z')){
    	return c;
    }

	return('_');
}

// Convert a filename into how it would appear on disk
// Returns the string following the filenane
static const char* _fileUserToFatName(const char* filename,char* fatfilename){
	boolean inXtn=FALSE; // are we doing the file extension
	boolean valid=FALSE; // is the file valid

	// Zap the filename
	memset(fatfilename,' ',11);

	uint8_t c=0;		// offset to write into the filename

	if(*filename == '.'){
		// Check for '.' or '..'
		fatfilename[0]='.';
		valid=TRUE;
		if(*(filename+1) == '.'){
			fatfilename[1]='.';
			filename+=2;
		}else{
			filename++;
		}
	}else{
		while(*filename && *filename != ' ' && *filename != '/'){
			if(*filename=='.' && !inXtn){
				// If we've found the first dot then move to extension
				inXtn=TRUE;
				c=8;
			}else{
				if(inXtn){
					if(c<=10){
						fatfilename[c]=_fileValidateChar(*filename);
						c++;
					}
				}else{
					if(c<=7){
						fatfilename[c]=_fileValidateChar(*filename);
						c++;
						valid=TRUE;
					}
				}
			}
			filename++;
		}
	}

	if(valid){
		if(*filename=='\0'){
			return(filename);
		}else{
			return(filename+1);
		}
	}else{
		return(0);
	}
}

// Try to locate a file with the given name
// Return value: Returns 0 when nothing was found, 1 when the thing found (and loc is filled in)
// was a file and 2 if the thing found was a directory. Also returns lstDir set to the cluster
// of the directory

int8_t _fileFindFile(const DISK* disk,const char* filename,DIR_POSITION *loc,CLUSTER *lastDir){
	CLUSTER fccd;		// First cluster of current directory
	CLUSTER tmpclus;
	char ffname[11];
	const char *next;
	boolean filefound=FALSE;

	if(*filename=='/'){
		fccd = _diskGetFirstClusterRootDir(disk);		// current directory = root
		filename++;
		if(lastDir){
			*lastDir=fccd;
		}
		if(*filename=='\0'){
			return(2);								// The filename is a directory
		}
	}else{
		fccd = disk->firstClusterCurrentDir;		// current directory = current directory
		if(lastDir){
			*lastDir=fccd;
		}
	}


	// Iterator
	uint8_t it=0;

	while((next=_fileUserToFatName(filename,ffname))!=0){

		if((tmpclus=_dirFindinDir(disk,ffname,fccd,loc,DIRFIND_FILE))==0){
			/* We didn't find what we wanted */
			/* We should check, to see if there is more after it, so that
			 * we can invalidate lastDir
			 */
			if((_fileUserToFatName(next,ffname))!=0){
				if(lastDir){
					*lastDir=0;
				}
			}
			return(0);
		}

		it++;

		if(loc->attrib.flags.isDirectory){
			fccd = tmpclus;
			filename = next;
			if(lastDir){
				*lastDir=fccd;
			}
			if(filefound){
				*lastDir=0;
			}
		}else{
			filefound=TRUE;
			if((_fileUserToFatName(next,ffname))!=0){
				if(lastDir){
					*lastDir=0;
				}
				return(0);
			}else{
				filename=next;
			}
		}
	}

	if(it==0){
		return(0);
	}

	if(loc->attrib.flags.isDirectory || !filefound){
		return(2);
	}
	return(1);
}


/*------------------------------------------------------------------------*/
/*					Directory Entry Management	  						  */
/*------------------------------------------------------------------------*/

// Attempt to locate a given filename in a director sector
// Return: 0 if not found, else the starting cluster for the file data
static CLUSTER _dirFindFileInSector(const DIR_ENTRY *fileEntry,const  char *fatname, DIR_POSITION *loc){

	for(uint8_t c=0; c<16; c++,fileEntry++){
		/* Check if the entry is for short filenames */
		if( (fileEntry->attribute.bits & 0x0F) != 0x0F ){
			if( memcmp((const char*)fileEntry->filename,fatname,sizeof(fileEntry->filename)) == 0 ){
				/* The entry has been found, return the location in the dir */
				if(loc){
					loc->entryInSector = c;
					loc->attrib = fileEntry->attribute;
				}

				CLUSTER firstCluster=(((CLUSTER )fileEntry->firstClusterHigh)<<16)+ fileEntry->firstClusterLow;

				if(firstCluster==0){
					return(1); /* Lie about cluster, 0 means not found! */
				}else{
					return firstCluster;
				}
			}
		}
	}
	return(0);
}


// This function searches for a free entry in a given directory sector buffer.
// It will put the offset into the loc->Offset field, given that loc is not 0.
// Return value: TRUE when it found a free spot, FALSE if it hasn't.
static uint32_t _dirFindFreeEntryinRAM(const DIR_ENTRY *fileEntry, DIR_POSITION *loc){
	for(uint8_t c=0;c<16;c++,fileEntry++){
		if( (fileEntry->attribute.bits & 0x0F) != 0x0F ){	// If its not a long filename entry
			if(fileEntry->filename[0] == 0x00 ||		// And is unused or
			   fileEntry->filename[0] == 0xE5 ){		// The file is deleted
				if(loc){
					loc->entryInSector=c;
				}
				return(1);								// We can use it
			}
		}
	}
	return(0);
}

// This function searches for a given fatfilename in a buffer.
// Return value: Returns 0 on not found, and the firstcluster when the name is found.
static CLUSTER _dirFindInSector(const DIR_ENTRY *fileEntry,const char *fatname, DIR_POSITION *loc, uint8_t mode){
	switch(mode){
		case DIRFIND_FILE:
			return(_dirFindFileInSector(fileEntry,fatname,loc));
		case DIRFIND_FREE:
			return(_dirFindFreeEntryinRAM(fileEntry,loc));
	}
	return(0);
}

// This function will search for an existing (fatname) or free directory entry
// in the rootdirectory-area of a FAT12/FAT16 filesystem.
// Return value: 0 on failure, firstcluster on finding file, and 1 on finding free spot.
static CLUSTER _dirFindinRootArea(const DISK *disk,const char* fatname, DIR_POSITION *loc, uint8_t mode){
	CLUSTER fclus;

	// FAT32 doesn't have a root area
	if((disk->fatType != 12) && (disk->fatType != 16))
		return(0);

	// Find the end sector of the root area
	SECTOR last = disk->firstSectorRootDir + disk->volume.rootDirEntryCount/32;

	for(SECTOR c=disk->firstSectorRootDir; c<last; c++){
		void* buf= _partitionGetSector(disk,c,BUFFER_MODE_READONLY);
		if((fclus=_dirFindInSector(buf,fatname,loc,mode))){
			if(loc){
				loc->sector=c;
			}
			partition_releaseSector(disk,buf);
			return(fclus);
		}
		partition_releaseSector(disk,buf);
	}
//	partition_releaseSector(disk,buf);
	return(0);
}

// This function will search for an existing (fatname) or free directory entry
// in a full cluster.
// Return value: 0 on failure, firstcluster on finding file, and 1 on finding free spot.

static CLUSTER _dirFindinCluster(const DISK *disk,CLUSTER cluster,const char *fatname, DIR_POSITION *loc, uint8_t mode){
	CLUSTER fclus;

	for(uint8_t c=0; c<disk->volume.sectorsPerCluster; c++){
		DIR_ENTRY* buf = _partitionGetSector(disk,_diskClusterToSector(disk,cluster)+c,BUFFER_MODE_READONLY);
		if((fclus=_dirFindInSector(buf,fatname,loc,mode))){
			if(loc){
				loc->sector = _diskClusterToSector(disk,cluster) + c;
			}
			partition_releaseSector(disk,buf);
			return(fclus);
		}
		partition_releaseSector(disk,buf);
	}
	return(0);
}



// This function will search for an existing (fatname) or free directory entry
// in a directory, following the clusterchains.
// Return value: 0 on failure, firstcluster on finding file, and 1 on finding free spot.

static CLUSTER _dirFindinDir(const DISK* disk, const char* fatname,CLUSTER firstcluster, DIR_POSITION *loc, uint8_t mode){
	CLUSTER_COUNT c=0;
	CLUSTER cluster;
	CLUSTER_NAV nav;

	_navInitClusterChain(&nav,firstcluster);

	if(firstcluster <= 1){
		return(_dirFindinRootArea(disk,fatname,loc,mode));
	}

	while( _fatNavigateTo(disk,&nav,c++) ){
		if((cluster=_dirFindinCluster(disk,nav.currentCluster,fatname,loc,mode))){
			return(cluster);
		}
	}
	return(0);
}

// This function will take a full directory path, and strip off all leading
// dirs and characters, leaving you with the MS-DOS notation of the actual filename.
// Return value: TRUE on success, FALSE on not being able to produce a filename

boolean _dirGetFatFileName(const char* filename, char* fatfilename){
	char ffnamec[11];
	const char *next;
	char nn=0;

	memclr(ffnamec,11);
	memclr(fatfilename,11);
	next = filename;

	if(*filename=='/'){
		next++;
	}

	while((next=_fileUserToFatName(next,ffnamec))){
//		memCpy(ffnamec,fatfilename,11);
		memcpy(fatfilename,ffnamec,11);
		nn++;
	}
	return (nn) ? TRUE : FALSE;
}

// This function extends a directory by 1 cluster
// It will also delete the contents of that
// cluster. (or clusters)
// Return value: TRUE on success, FALSE on fail

static boolean _dirAddCluster(DISK* disk,CLUSTER firstCluster){
	CLUSTER_NAV nav;
	_navInitClusterChain(&nav,firstCluster);
	if(!_fatExtend(disk,&nav,1)){
		return(FALSE);
	}
	CLUSTER lastc = _navGetLastCluster(disk,&nav);
	_partitionClearCluster(disk,lastc);
	return(TRUE);
}

// Write a directory entry
void _dirCreateDirectoryEntry(const DISK* disk,const DIR_ENTRY *filerec,const DIR_POSITION *loc){
	DIR_ENTRY* dir = _partitionGetSector(disk,loc->sector,BUFFER_MODE_READWRITE);
	memcpy(&dir[loc->entryInSector],filerec,sizeof(*filerec));
	partition_releaseSector(disk,dir);
}

boolean _dirFindFreeFile(DISK* disk,const char* filename,DIR_POSITION *loc){
	CLUSTER targetdir=0;
	char ffname[11];

	if(_fileFindFile(disk,filename,loc,&targetdir)){
		// The file already exists
		return(FALSE);
	}

	// Parse for a valid filename
	if(!_dirGetFatFileName(filename,ffname)){
		return(FALSE);
	}

	if(_dirFindinDir(disk,ffname,targetdir,loc,DIRFIND_FREE)){
		return(TRUE);
	}else{
		// Try to increase the directory size
		if(!_dirAddCluster(disk,targetdir)){
			return(FALSE);
		}else{
			if(_dirFindinDir(disk,ffname,targetdir,loc,DIRFIND_FREE)){
				return(TRUE);
			}
		}
	}

	return(FALSE);
}

// Update the disk directory with the start cluster
void _dirSetFirstClusterInDirEntry(DIR_ENTRY *rec,CLUSTER cluster_addr){
	rec->firstClusterHigh=cluster_addr>>16;
	rec->firstClusterLow=cluster_addr&0xFFFF;
}

/*------------------------------------------------------------------------*/
/*					Cluster Navigation Management  						  */
/*------------------------------------------------------------------------*/

// Find the last cluster in a cluster chain and store it in the cache
static CLUSTER _navGetLastCluster(const DISK* disk,CLUSTER_NAV *nav){
	// If not initialised then go to start of file
	if(nav->currentCluster==0){
		nav->currentCluster=nav->startCluster;
		nav->relativeCluster=0;
	}

	// If we haven't found the last cluster yet then find it now
	if(nav->endCluster==0){
		while(_fatGetNextClusterChain(disk, nav)){
			nav->relativeCluster+=nav->contiguousCount;
			nav->currentCluster+=nav->contiguousCount;
			nav->contiguousCount=0;
		}
	}
	return(nav->endCluster);
}

void _navInitClusterChain(CLUSTER_NAV *nav,CLUSTER cluster_addr){
	memclr(nav,sizeof(CLUSTER_NAV));
	nav->startCluster = nav->currentCluster=cluster_addr;
}





