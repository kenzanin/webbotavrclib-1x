/*
 * $Id: FAT.h,v 1.4 2011/07/06 00:18:31 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FAT.h,v $
 * Revision 1.4  2011/07/06 00:18:31  clivewebster
 * Add PROGMEM to class def and rename FILE to FATFILE
 *
 * Revision 1.3  2010/06/14 19:16:07  clivewebster
 * Add copyright license info
 *
 * Revision 1.2  2010/03/20 00:46:06  clivewebster
 * *** empty log message ***
 *
 * Revision 1.1  2010/03/08 16:11:13  clivewebster
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
 * FAT.h
 *
 *  Created on: 28 Feb 2010
 *      Author: Clive Webster
 *
 * Exposes the external API for the filesystem
 */

#ifndef FAT_H_
#define FAT_H_

#include "../../libdefs.h"

typedef uint32_t CLUSTER;			// A cluster number
typedef uint32_t CLUSTER_COUNT;		// A number of clusters
typedef uint32_t SECTOR;			// A sector number
typedef uint32_t SECTOR_COUNT;		// A number of sectors


/*------------------------------------------------------------------
 *  Define the interface for the underlying storage device
------------------------------------------------------------------*/
typedef struct /*PROGMEM*/ c_storage{
	boolean (*read)(void* device,SECTOR absSector,void* dta);
	boolean (*write)(void* device, SECTOR absSector,const void* dta);
	SECTOR_COUNT (*getTotalSectors)(void* device);
} STORAGE_CLASS;

#define MAKE_STORAGE_CLASS(read, write,getTotalSectors) { \
	read,write,getTotalSectors }



/*------------------------------------------------------------------
 *  Define structures for elements stored on disk
 *  DO NOT CHANGE - YOU WILL CORRUPT THE DISK !!
------------------------------------------------------------------*/
// The layout of a volume on the disc
typedef struct s_volume{
	uint16_t 	bytesPerSector;		// bytes per sector - must be 512
	uint8_t 	sectorsPerCluster;	// the number of sectors in a cluster
	uint16_t 	reservedSectorCount;// the number of reserved sectors at the start of the volume
	uint8_t 	numberOfFats;		// how many copies of the FAT there are
	uint16_t 	rootDirEntryCount;	// Number of file entries in the root directory
	uint32_t 	rootDirCluster;		// For FAT32 the cluster of the root directory
}VOLUME;

typedef struct s_dir_attr{
	union{
		uint8_t bits;
		struct {
			uint8_t  isReadOnly:1;
			uint8_t  isHidden:1;
			uint8_t  isSystem:1;
			uint8_t	 isVolume:1;
			uint8_t  isDirectory:1;
			uint8_t  isArchive:1;
			uint8_t	 unused:2;
		} flags;
	};
}DIR_ATTR;

// This is the layout of a directory entry on the disk
typedef struct s_dir_entry{
	char     filename[11];			// Filename
	DIR_ATTR attribute;				// Bitwise OR of the file attributes
	uint8_t  NTReserved;
	uint8_t  milliSecTimeStamp;
	uint16_t creationTime;			// Time and ...
	uint16_t creationDate;			// Date when created
	uint16_t lastAccessDate;		// Date when last accessed
	uint16_t firstClusterHigh;		// The top 16 bits of the first cluster
	uint16_t writeTime;				// Time and ...
	uint16_t writeDate;				// Date when last modified
	uint16_t firstClusterLow;		// The bottom 16 bits of the first cluster
	uint32_t fileSize;				// The used filesize in bytes
} DIR_ENTRY;


/*------------------------------------------------------------------
 *  Define internal data structures
------------------------------------------------------------------*/
// Structure for navigating all clusters in a file/directory
typedef struct s_cluster_nav{
	CLUSTER_COUNT	contiguousCount;	// The number of contiguous clusters
	CLUSTER_COUNT   relativeCluster;	// The current cluster offset count from start of the file
	CLUSTER  		currentCluster;		// The 'relativeCluster' after 'startCluser', or 0 if the structure is invalid
	CLUSTER  		startCluster;		// The start cluster number in the chain (Must be > 1)
	CLUSTER  		endCluster;			// The end cluster in the chain (if found yet)
	CLUSTER_COUNT	totalClusters;		// The total number of clusters in the chain (if found yet)
} CLUSTER_NAV;

// The details on a sector buffer
typedef struct s_sector_buffer{
	SECTOR   sector;					// The sector # in the buffer
	uint8_t  usage;						// The active usage count
	uint8_t  isValid:1;					// Is this a valid entry
	uint8_t  isWritable:1;				// Does it need writing to disk
} SECTOR_BUFFER;

// The details on one buffer
#define MAX_STACK_DEPTH 4
typedef struct s_buffer{
	uint8_t		reference;					// The number of times this buffer has been accessed
	uint8_t		depth;						// The number of items on the stack
	SECTOR_BUFFER stack[MAX_STACK_DEPTH];	// The stack
	uint8_t		dta[512];					// The disk transfer address
} BUFFER;

// Stores the location of a directory entry on the disk
typedef struct s_file_location{
	SECTOR 	sector;							// The sector for the directory info
	uint8_t entryInSector;					// The zero relative entry number in the sector
	DIR_ATTR attrib;						// The file attribute of the entry
} DIR_POSITION;


/*------------------------------------------------------------------
 *  Define external data structures
------------------------------------------------------------------*/

// The detail of a disk - set by disk_Init
typedef struct s_disk{
	const STORAGE_CLASS* sclass;		// The code for reading/writing sectors
	void*		device;				// The hardware device
	boolean		initialised;		// Has it been initialised?

	// Partition info (constant)
	uint8_t		partitionType;		// The active partition type as stored on disk
	SECTOR 		partitionStartSector;// Disk sector offset to the first sector of this partition
	SECTOR_COUNT partitionNumSectors;// Total number of sectos in the partition

	// Volume info (constant)
	VOLUME		volume;
	SECTOR_COUNT fatSectorCount;		// The number of sectors used by the FAT table
	SECTOR_COUNT sectorCount;			// The total number of sectors
	CLUSTER_COUNT dataClusterCount;		// The total number of clusters
	uint8_t		fatType;				// 12, 16 or 32 bit FAT?
	SECTOR		firstSectorRootDir; 	// The first sector for the root directory
	CLUSTER		firstClusterCurrentDir; // The first cluster of the current directory

	// Working info(variable)
	CLUSTER		freeClusterHint;		// Where to start looking for free clusters

	// Memory buffers
	uint8_t 	numbuf;					// The number of sector buffers in RAM
	BUFFER*		buffers;				// The buffer array
} DISK;

// Used by file_findFirst/Next
typedef struct s_file_iterator{
	// Returned values
	char 	 filename[13];		// Stored as null terminated string
	uint32_t fileSize;			// Byte size of the file
	DIR_ATTR  attribute;		// The file attributes

	// Internal use
	const DISK *disk;			// Remember wihich disk was being accessed
	uint16_t 	dirEntry;		// The current directory entry
	CLUSTER_NAV nav;			// Remember where we are on the disk
} FILE_ITERATOR;

// The details on a FILE
typedef struct s_file{
	DISK * 		disk;				/* The disk it is stored on							*/
	CLUSTER		startCluster;		/* Start cluster of the file data					*/
	DIR_POSITION directory; 		/* Location in directory 							*/
	CLUSTER_NAV nav;				/* Details on the current cluster navigation		*/
	uint8_t		mode;				/* Bitwise OR of how to access the file				*/
	uint32_t 	filePos;			/* The current location to read/write				*/
	uint32_t 	fileSize;			/* The current file length in bytes					*/
} FATFILE;




/*------------------------------------------------------------------
 *  Define the available methods
------------------------------------------------------------------*/



/*-----------------------------------------------------------------
 Initialise a disk
 If the 'device' is an SD_CARD then 'sclass' should be '&c_sdCard_disk'
 Note that the device should already have been initialised
 Return: TRUE if ok, FALSE if not
-----------------------------------------------------------------*/
boolean diskInit(DISK *disc, uint8_t numBuffers, const STORAGE_CLASS* sclass, void* device);

/*-----------------------------------------------------------------
    Flush all changes out to the disk. This should be called at the
   end of your program (if there is such a thing) or if you want to
   unmount the drive
   Return TRUE if flushed ok
-----------------------------------------------------------------*/
boolean diskFlush(const DISK *disk);

/*-----------------------------------------------------------------
   Find the amount of freespace - can take a while to run
   Return The number of free KB on the disk
-----------------------------------------------------------------*/
uint32_t diskFreeSpace(const DISK* disk);

/*-----------------------------------------------------------------
	Create a directory
	Return: -1 if there is already a file or directory with this name
			-2 if the directory is full
			-3 if the disk is full
			0  if all ok
-----------------------------------------------------------------*/
int8_t diskMkdir(DISK* disk,const char* dirname);

/*-----------------------------------------------------------------
	Find the first entry in the specified directory
	Return: FALSE if none found
			else the FILE_ITERATOR contains info on the file
-----------------------------------------------------------------*/
boolean fileFindFirst(const DISK* disk,FILE_ITERATOR *list,const char* dirname);
/*-----------------------------------------------------------------
	Find the next entry in the specified directory
	Return: FALSE if none found
			else the FILE_ITERATOR contains info on the file
-----------------------------------------------------------------*/
boolean fileFindNext(FILE_ITERATOR *list);

/*-----------------------------------------------------------------
	Open a file for sequential or random access
	The mode can be 'r' for read, 'w' for write, 'a' for append
	Return:  0 if ok
			-1 if the file doesn't exist (when opening for read)
			-2 if the file already exists (when opening for write)
			-3 if the directory doesnt exist (when opening for write)
			-4 if trying to append to a read only file
			-5 if there is no disk space to create the file
			-6 if the mode is invalid
-----------------------------------------------------------------*/
int8_t fileOpen(DISK* disk, FATFILE* file,const char* filename,char mode);

/*-----------------------------------------------------------------
	Flush a file to disk without having to close it
	It is recommended that you call this on a regular basis to
	make sure that the data is committed to disk
-----------------------------------------------------------------*/
void fileFlush(FATFILE* file);

/*-----------------------------------------------------------------
	Flush and close a file. Note that if you unplug the disk or
	turn off the power without closing, or flushing, a file then
	its contents may be lost.
-----------------------------------------------------------------*/
void fileClose(FATFILE *file);

/*-----------------------------------------------------------------
	Change the position in the file that you are reading or
	writing from.
	Return: FALSE if you are trying to move beyond the end of the file
-----------------------------------------------------------------*/
boolean fileSetPos(FATFILE *file,uint32_t pos);

/*-----------------------------------------------------------------
	Return: The current file position
-----------------------------------------------------------------*/
static __inline__ uint32_t fileGetPos(const FATFILE* file){
	return file->filePos;
}

/*-----------------------------------------------------------------
	Return: The current file size in bytes
-----------------------------------------------------------------*/
static __inline__ uint32_t fileGetSize(const FATFILE* file){
	return file->fileSize;
}

/*-----------------------------------------------------------------
	Perform a (random) read from anywhere within the file.
	NB This doesn't use the current file position and will not update it
	Return: The number of bytes actually read (or 0 if at end of file)
-----------------------------------------------------------------*/
size_t fileRead(FATFILE *file,uint32_t offset, size_t size,void *buf);

/*-----------------------------------------------------------------
	Do a (sequential) read from the current file position and update
    the file position to the new position.
	Return: The number of bytes actually read (or 0 if at end of file)
-----------------------------------------------------------------*/
size_t fileReadNext(FATFILE *file,size_t size,void *buf);

/*-----------------------------------------------------------------
	Perform a (random) write to anywhere within the file or may extend it.
	NB This doesn't use the current file position and will not update it
	Return: The number of bytes actually written
-----------------------------------------------------------------*/
size_t fileWrite(FATFILE* file,uint32_t offset,size_t size,const void* buf);

/*-----------------------------------------------------------------
	Do a (sequential) write at the current file position and update
    the file position to the new position.
	Return: The number of bytes actually written
-----------------------------------------------------------------*/
size_t fileWriteNext(FATFILE *file, size_t size,const void *buf);

/*-----------------------------------------------------------------
	Return: TRUE if the specified file/directory already exists
-----------------------------------------------------------------*/
boolean fileExists(DISK * disk,const char* filename);

/*-----------------------------------------------------------------
    Delete the specified file
	Return: FALSE if the file doesn't exist or is read only
-----------------------------------------------------------------*/
boolean fileDelete(DISK * disk,const char* filename);

/*-----------------------------------------------------------------
    Obtain a writer so that you can use rprintf to write sequentially
    to a file
	Return: The new writer, which you will then use with rprintfInit
-----------------------------------------------------------------*/
Writer  fileGetWriter(const FATFILE* file);

#endif /* FAT_H_ */
