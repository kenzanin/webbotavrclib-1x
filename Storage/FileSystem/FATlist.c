
/*
 * $Id: FATlist.c,v 1.3 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATlist.c,v $
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
 * FATlist.c
 *
 *  Created on: 3 Mar 2010
 *      Author: Clive Webster
 */
#include "FATImpl.h"


static void _fileEntryToDirListEntry(FILE_ITERATOR *dlist, const DIR_ENTRY* buf, uint16_t offset){
	buf+=offset;
	uint8_t srcpos = 0;
	uint8_t dstpos = 0;

	while(srcpos < 8){
		char c = buf->filename[srcpos];
		if(c != ' ' && c != '\0'){
			dlist->filename[dstpos++] = c;
		}
		srcpos++;
	}
	dlist->filename[dstpos++] = '.';

	while(srcpos < 8+3){
		char c = buf->filename[srcpos];
		if(c != ' ' && c != '\0'){
			dlist->filename[dstpos++] = c;
		}
		srcpos++;
	}

	if(dlist->filename[dstpos-1] == '.'){
		dstpos--;
	}
	dlist->filename[dstpos] = '\0';

	dlist->attribute = buf->attribute;
	dlist->fileSize = buf->fileSize;
}

static boolean _getRootAreaEntry(FILE_ITERATOR *dlist){
	if((dlist->disk->fatType != 12) && (dlist->disk->fatType != 16)){
		return(FALSE);
	}
	if(dlist->dirEntry>=dlist->disk->volume.rootDirEntryCount){
		return(FALSE);
	}

	DIR_ENTRY* buf = _partitionGetSector(dlist->disk,
					   dlist->disk->firstSectorRootDir+dlist->dirEntry/16,
					   BUFFER_MODE_READONLY);
	_fileEntryToDirListEntry(dlist,buf,(dlist->dirEntry%16));
	partition_releaseSector(dlist->disk,buf);
	return(TRUE);
}


static boolean _getRealDirEntry(FILE_ITERATOR *dlist){

	if(dlist->nav.startCluster<=1){
		return(FALSE);
	}

	CLUSTER_COUNT clusterInDir = (dlist->dirEntry)/(16 * dlist->disk->volume.sectorsPerCluster);
	if(_fatNavigateTo(dlist->disk,
						   &(dlist->nav),
						   clusterInDir)==FALSE){
		return(FALSE);
	}

	SECTOR sector = _diskClusterToSector(dlist->disk,dlist->nav.currentCluster) +
					(dlist->dirEntry/16)%dlist->disk->volume.sectorsPerCluster;

	DIR_ENTRY* buf = _partitionGetSector(dlist->disk,sector,BUFFER_MODE_READONLY);
	_fileEntryToDirListEntry(dlist,buf,(dlist->dirEntry%16));
	partition_releaseSector(dlist->disk,buf);

	return(TRUE);
}

static boolean _getDirEntry(FILE_ITERATOR *dlist){
	if(dlist->nav.startCluster == 1){
		return(_getRootAreaEntry(dlist));
	}else if(dlist->nav.startCluster){
		return(_getRealDirEntry(dlist));
	}
	return(FALSE);
}

static boolean _isValidFileEntry(FILE_ITERATOR *entry){
	if(entry->filename[0] == 0 || entry->filename[0] == 0xE5 || entry->filename[0] == '.'){
		return FALSE;
	}
	if((entry->attribute.bits & 0x0F)==0x0F){
		return FALSE;						// Ignore it if its a long filename entry
	}
 	return TRUE;
}


// Get the next directory entry
boolean fileFindNext(FILE_ITERATOR *dlist){
	do{
		if(!_getDirEntry(dlist)){
			return FALSE;
		}
		dlist->dirEntry++;

	}while(!_isValidFileEntry(dlist));

	return TRUE;
}

// Open a directory
// This will fill in the FILE_ITERATOR
// Returns: FALSE if it cannot find the directory
// else use ls_getNext to iterate through each file name in the directory
boolean fileFindFirst(const DISK* disk,FILE_ITERATOR *dlist,const char* dirname){
	DIR_POSITION loc;
	CLUSTER directoryCluster;

	dlist->disk=disk;

	if(_fileFindFile(disk,dirname,&loc,&directoryCluster)!=2){
		// Cant find directory
		return FALSE;
	}


	_navInitClusterChain(&(dlist->nav),directoryCluster);

	dlist->filename[0] = '\0';
	dlist->dirEntry=0;

	return fileFindNext(dlist);
}

