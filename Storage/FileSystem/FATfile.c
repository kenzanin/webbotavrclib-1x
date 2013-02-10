
/*
 * $Id: FATfile.c,v 1.4 2011/07/06 00:18:57 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATfile.c,v $
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
 * FATfile.c
 *
 *  Created on: 3 Mar 2010
 *      Author: Clive Webster
 *
 *  File open, read, write, close
 */

#include "FATImpl.h"
#include <stdlib.h>

#define MODE_READ 'r'
#define MODE_WRITE 'w'
#define MODE_APPEND 'a'

// Bit values for FileStatus
#define FILE_STATUS_OPEN 0
#define FILE_STATUS_WRITE 1

FATFILE* activeFile;

// Update a disk directory entry with the file size
static void _dirSetFileSize(const DISK *disk, DIR_POSITION *loc,uint32_t numbytes){
	DIR_ENTRY* dir = _partitionGetSector(disk,loc->sector,BUFFER_MODE_READWRITE);
	dir[loc->entryInSector].fileSize=numbytes;
	partition_releaseSector(disk,dir);
}


// This function stores the filerecord located at loc in filerec.
// It fetches the required sector for this.
// Return value: void
static void _dirGetFileStructure(const DISK* disk,DIR_ENTRY *filerec,const DIR_POSITION *loc){
	void* buf=_partitionGetSector(disk,loc->sector,BUFFER_MODE_READONLY);
	DIR_ENTRY* dir = (DIR_ENTRY*)buf;
	memcpy(filerec,&dir[loc->entryInSector],sizeof(DIR_ENTRY));
	partition_releaseSector(disk,buf);
}

// This function fills in a filerecord with safe default values, and
// a given fatfilename. If your system has a means of knowing time, here is an
// excellent place to apply it to the filerecord.
// Return value: void
static void _dirCreateDefaultEntry(DIR_ENTRY *filerec,const char* fatfilename){
	memclr(filerec,sizeof(DIR_ENTRY));
	memcpy(filerec->filename,fatfilename,11);
}


// Initialises a new file object
static void _fileInitFile(DISK* disk, FATFILE *file, const DIR_ENTRY* dir, const DIR_POSITION *loc){
	file->disk=disk;
	file->fileSize=dir->fileSize;
	file->filePos=0;
	file->directory.sector=loc->sector;
	file->directory.entryInSector=loc->entryInSector;

	CLUSTER firstCluster = (((CLUSTER)dir->firstClusterHigh)<<16)+dir->firstClusterLow;;
	file->startCluster = firstCluster;

	_navInitClusterChain(&file->nav, firstCluster);
}

boolean fileSetPos(FATFILE *file,uint32_t pos){
	if(pos<=file->fileSize){
		file->filePos=pos;
		return(TRUE);
	}
	return(FALSE);
}

static uint8_t _fileGetAttr(FATFILE* file,uint8_t attribute){
	return(file->mode&(1<<attribute));
}

static void _fileSetAttr(FATFILE* file,uint8_t attribute,uint8_t val){
	if(val){
		file->mode|=1<<attribute;
	}else{
		file->mode&=~(1<<attribute);
	}
}

// Update a disk directory entry with the given start cluster
static void _dirSetFirstCluster(const DISK* disk,const DIR_POSITION *loc,CLUSTER cluster_addr){
	DIR_ENTRY* dir = _partitionGetSector(disk,loc->sector,BUFFER_MODE_READWRITE);
	dir[loc->entryInSector].firstClusterHigh=cluster_addr>>16;
	dir[loc->entryInSector].firstClusterLow=cluster_addr&0xFFFF;
	partition_releaseSector(disk,dir);
}


static void _allocateFirstCluster(DISK* disk, FATFILE* file, DIR_ENTRY* dir, const DIR_POSITION *loc){
	CLUSTER cluster=_fatGetFreeCluster(disk);
	if(cluster != 0){
		// Update disk dir with first cluster
		_dirSetFirstCluster(disk,&(file->directory),cluster);

		// Update directory with first cluster
		_dirSetFirstClusterInDirEntry(dir,cluster);

		// First cluster is end of chain
		_fatSetNextClusterAddress(disk,cluster,_fatGiveEndMarker(disk));

		// Re-initialise the file with the changes
		_fileInitFile(disk,file,dir,loc);
	}
}

boolean fileExists(DISK * disk,const char* filename){
    DIR_POSITION loc;
    char fatfilename[11];
    _dirGetFatFileName(filename,fatfilename);
    return (_fileFindFile(disk,filename,&loc,null)==0) ? FALSE : TRUE;
}

// Open a file and return a file handle 'File'
int8_t fileOpen(DISK* disk, FATFILE* file,const char* filename,char mode){
    DIR_POSITION loc;
    char fatfilename[11];
    CLUSTER dirCluster;
	DIR_ENTRY wtmp;

    _dirGetFatFileName(filename,fatfilename);

    switch(mode){
        case MODE_READ:
            if(_fileFindFile(disk,filename,&loc,null)==1){
                _dirGetFileStructure(disk,&wtmp, &loc);
                _fileInitFile(disk,file,&wtmp,&loc);
				_fileSetAttr(file,FILE_STATUS_OPEN,1);
				_fileSetAttr(file,FILE_STATUS_WRITE,0);
                return(0);
            }
            return(-1);
        case MODE_WRITE:
            if(_fileFindFile(disk,filename,&loc,&dirCluster)){
                return(-2);	// File already exists
			}
			if(dirCluster==0){ 	// Parent dir does not exist
 				return(-3);
			}
			goto create;
        case MODE_APPEND:
			if(_fileFindFile(disk,filename,&loc,0)==1){ /* File exists */
				if(loc.attrib.flags.isReadOnly){
					return -4;						  /* but is read only */
				}
				_dirGetFileStructure(disk,&wtmp, &loc);
				_fileInitFile(disk,file,&wtmp,&loc);
				if(file->nav.startCluster==0){
					// The file is empty
					_allocateFirstCluster(disk,file,&wtmp,&loc);
				}
				fileSetPos(file,file->fileSize);
				_fileSetAttr(file,FILE_STATUS_OPEN,1);
				_fileSetAttr(file,FILE_STATUS_WRITE,1);
			}else{ /* File does not excist */
create:
				// Create a new file
				if(_dirFindFreeFile(disk,filename,&loc))	{
					_dirCreateDefaultEntry(&wtmp,fatfilename);
					// Copy directory to disk
					_dirCreateDirectoryEntry(disk,&wtmp,&loc);

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("Create dir '%11.11s' @%lu Entry=%u\n",fatfilename,loc.sector,loc.entryInSector);
		rprintfInit(old);}
#endif


					// Initialise file with directory info
					_fileInitFile(disk,file,&wtmp, &loc);

					// Allocate the first cluster to the file data
					_allocateFirstCluster(disk,file,&wtmp,&loc);

					_fileSetAttr(file,FILE_STATUS_OPEN,1);
					_fileSetAttr(file,FILE_STATUS_WRITE,1);
				}else{
					return(-5);
				}
			}
			return(0);
    }
    return(-6);
}


// Calculate the number of clusters starting with a given one
static CLUSTER_COUNT _fatCountClustersInChain(const DISK *disk,CLUSTER firstcluster){

	if(firstcluster<=1){
		return(0);
	}

	// Initialise to start of chain
	CLUSTER_NAV nav;
	_navInitClusterChain(&nav,firstcluster);

	// Step through each successive cluster
	CLUSTER_COUNT c=0;
	while( _fatNavigateTo(disk,&nav,c++) );

	// Return the count
	return(c-1);
}

void fileFlush(FATFILE* file){
	if(_fileGetAttr(file,FILE_STATUS_WRITE)){

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		CLUSTER_COUNT cnt = _fatCountClustersInChain(file->disk,file->startCluster);
		rprintf("Flush File: First Cl=%lu, NumCl=%lu\n",file->startCluster,cnt);
		rprintfInit(old);}
#endif

		_dirSetFileSize(file->disk,&(file->directory),file->fileSize);
		// Flush everything for now
		diskFlush(file->disk);
	}
}

void fileClose(FATFILE *file){
	fileFlush(file);

	if(activeFile==file){
		activeFile = null;
	}

	memclr(file,sizeof(*file));
//	file_setAttr(file,FILE_STATUS_OPEN,0);
//	file_setAttr(file,FILE_STATUS_WRITE,0);
}


// Determine the number of clusters that neeed to be added to the end of the file
CLUSTER_COUNT _fileRequiredCluster(FATFILE *file,uint32_t offset, size_t size){
	CLUSTER_COUNT clusters_required=0;

	uint32_t endPos = offset + (uint32_t)size;

	if(endPos > file->fileSize){
		// Find the number of clusters allocated to the file
		CLUSTER_COUNT hc;
		if(file->nav.totalClusters==0){ /* Number of cluster unknown */
			hc = _fatCountClustersInChain(file->disk,file->nav.startCluster);
			file->nav.totalClusters = hc;
		}else{
			hc = file->nav.totalClusters; /* This better be right */
		}

		// Find the clusters required for the file length.
		// NB 'hc' may be bigger than this if files are extended by more
		// than one cluster at a time
		uint32_t clustersize = file->disk->volume.bytesPerSector *
							   file->disk->volume.sectorsPerCluster;

		// Find the total number of clusters for the current file size
		CLUSTER currentClusterSize = (file->fileSize+clustersize-1) / clustersize;
		if( (endPos-file->fileSize) >
		   ((hc-currentClusterSize)*clustersize)){
			clusters_required = ((endPos-(hc*clustersize))+clustersize-1)/clustersize;
		}
	}
	return(clusters_required);
}


// This function reads 'size' bytes from 'file' starting at
// 'offset' and puts the result in '*buf'.
// This will not modify the current file position but can be used for random reads
// Return value: amount of bytes actually read (can differ from the given
// size when the file was smaller)
size_t fileRead(FATFILE *file,uint32_t offset, size_t size,void *buf){
	size_t bytes_read=0;			// The number of bytes read
	size_t size_left=size;			// The number of bytes left to read

	uint32_t currentOffset=offset;		// The current file position

	// Check that the file has been opened
	if(!_fileGetAttr(file,FILE_STATUS_OPEN)){
		return(0);
	}

	// Dont read beyond end of file
	if(offset>=file->fileSize){
		size_left=0; /* Offset check */
	}

	// Limit reads from going beyond end of file
	if( (offset+size > file->fileSize) && size_left!=0){
		size_left=file->fileSize-offset;
	}

	while(size_left>0){

		size_t offsetInSector = ((size_t)(currentOffset)) & 511;	// Remainder
		SECTOR relativeSector = (currentOffset >> 9);				// Divide by 512
		ldiv_t div = ldiv(relativeSector, file->disk->volume.sectorsPerCluster);
		CLUSTER_COUNT currentCluster = div.quot;
		SECTOR        sectorInCluster  = div.rem;


		// Calculate the number of bytes required to read from the sector
		size_t bytesToRead;
		if(offsetInSector!=0 || size_left<512){
			bytesToRead = (512-offsetInSector >= size_left) ? size_left : 512-offsetInSector;
		}else{
			bytesToRead = 512;
		}

		if((_fatNavigateTo(file->disk,&(file->nav),currentCluster))==FALSE){
			return(0);													// Cluster out of bounds
		}

		// Calc the sector# in the partition
		SECTOR readSector=_diskClusterToSector(file->disk,file->nav.currentCluster) + sectorInCluster;

		if(bytesToRead==512){
			// Read it via the buffers or directly to user RAM
			_partitionDirectSectorRead(file->disk,readSector,((uint8_t*)buf)+bytes_read);
		}else{
			// Read it into the buffers
			uint8_t* tbuf = _partitionGetSector(file->disk,readSector,BUFFER_MODE_READONLY);
			// Copy data from buffer into user memory
			memcpy(((uint8_t*)buf)+bytes_read, tbuf+offsetInSector,  bytesToRead);
			// Release the sector
			partition_releaseSector(file->disk,tbuf);
		}

		currentOffset += bytesToRead;
		bytes_read += bytesToRead;
		size_left -= bytesToRead;
	}

	return(bytes_read);
}

size_t fileReadNext(FATFILE *file,size_t size,void *buf){
	size_t r=fileRead(file,file->filePos,size,buf);
	file->filePos+=r;
	return(r);
}


// This function writes to a file, at offset 'offset' and size 'size'.
// It also updates the FileSize in the object, and disk directory
// Returns Bytes actually written.
size_t fileWrite(FATFILE* file,uint32_t offset,size_t size,const void* buf){
	uint32_t currentOffset=offset;

	// Make sure file is open for write
	if(!_fileGetAttr(file,FILE_STATUS_OPEN) || !_fileGetAttr(file,FILE_STATUS_WRITE)){
		return(0);
	}

	// Put offset into range
	if(offset>file->fileSize){
		offset=file->fileSize;
	}

	// Find if need to allocate more clusters to the end of the file
	CLUSTER_COUNT need_cluster = _fileRequiredCluster(file,offset,size);
	if(need_cluster){

#ifdef FAT_DEBUG
	{ Writer old = rprintfInit(uartGetWriter(FAT_DEBUG));
		rprintf("\nWrite to file@%lu, extend by %lu clusters\n",currentOffset,need_cluster);
		rprintfInit(old);}
#endif

		if(!_fatExtend(file->disk,&(file->nav),need_cluster)){
			// FAT is full
			return(0);
		}
	}

	size_t size_left=size;
	size_t bytes_written=0;
	while(size_left>0){

		size_t offsetInSector = ((size_t)(currentOffset)) & 511;	// Remainder
		SECTOR relativeSector = (currentOffset >> 9);				// Divide by 512
		ldiv_t div = ldiv(relativeSector, file->disk->volume.sectorsPerCluster);
		CLUSTER_COUNT currentCluster = div.quot;
		SECTOR        sectorInCluster  = div.rem;

		// calc the number of bytes required from the current sector
		size_t bytesToWrite;
		if(offsetInSector!=0 || size_left<512){
			bytesToWrite = (512-offsetInSector>=size_left) ? size_left : 512-offsetInSector;
		}else{
			bytesToWrite = 512;
		}

		if((_fatNavigateTo(file->disk,&(file->nav),currentCluster))==FALSE){
			// cluster is out of bounds
			file->fileSize+=bytes_written;
			_dirSetFileSize(file->disk,&(file->directory),file->fileSize);
			return(bytes_written);
		}

		SECTOR writeSector=_diskClusterToSector(file->disk,file->nav.currentCluster) + sectorInCluster;

		if(bytesToWrite==512){
			// Write directly from user memory to the device
			_partitionDirectSectorWrite(file->disk,writeSector,((const uint8_t*)buf)+bytes_written);
		}else{
			// read the sector into the buffer for read/write
			uint8_t* tbuf = _partitionGetSector(file->disk,writeSector,BUFFER_MODE_READWRITE);
			// copy the user memory into the buffer
			memcpy(tbuf+offsetInSector,((const uint8_t*)buf)+bytes_written,bytesToWrite);
			// release the sector
			partition_releaseSector(file->disk,tbuf);
		}

		currentOffset += bytesToWrite;
		bytes_written += bytesToWrite;
		size_left -= bytesToWrite;
	}

	// Update the file size
	if(bytes_written > file->fileSize - offset){
		file->fileSize += bytes_written - (file->fileSize-offset);
   }

	return(bytes_written);
}
size_t fileWriteNext(FATFILE *file, size_t size,const void *buf){
	size_t r=fileWrite(file,file->filePos,size,buf);
	file->filePos+=r;
	return(r);
}

static MAKE_WRITER(writer){
	if(activeFile){
		fileWriteNext(activeFile,1,&byte);
	}
	return byte;
}

Writer fileGetWriter(const FATFILE* file){
	activeFile = (FATFILE*)file;
	return &writer;
}
