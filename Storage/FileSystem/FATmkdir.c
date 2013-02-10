
/*
 * $Id: FATmkdir.c,v 1.3 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATmkdir.c,v $
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
 * FATmkdir.c
 *
 *  Created on: 4 Mar 2010
 *      Author: Clive Webster
 */
#include "FATImpl.h"

int8_t diskMkdir(DISK* disk,const char* dirname){
	DIR_POSITION loc;
	DIR_ENTRY direntry;
	CLUSTER parentdir;
	char ffname[11];

	if( _fileFindFile(disk,dirname,&loc,&parentdir) ){
		// File or Dir already exists
		return(-1);
	}

	if(parentdir==0){
		// Cannot create a new root
		return(-1);
	}

	if(!_dirFindFreeFile(disk,dirname,&loc)){
		// No directory space
		return(-2);
	}

	CLUSTER nc = _fatGetFreeCluster(disk);
	if(nc==0){
		// No space to create the new directory area
		return(-3);
	}

	_partitionClearCluster(disk,nc);


	// Create it in the parent directory
	_dirGetFatFileName(dirname,ffname);
	memclr(&direntry,sizeof(direntry));
	memcpy(&direntry,ffname,11);
	_dirSetFirstClusterInDirEntry(&direntry,nc);
	direntry.attribute.flags.isDirectory = 1;
	_dirCreateDirectoryEntry(disk,&direntry,&loc);

	// Now add the '.' and '..' entries into the new directory
	DIR_ENTRY* dir = _partitionGetSector(disk,_diskClusterToSector(disk,nc),BUFFER_MODE_READWRITE);
	memclr(&direntry,sizeof(direntry));
	memcpy(&direntry,".          ",11);
	direntry.attribute.flags.isDirectory = 1;
	_dirSetFirstClusterInDirEntry(&direntry,nc);
	memcpy(&dir[0],&direntry,sizeof(DIR_ENTRY));

	if(disk->fatType == 32 && parentdir == disk->volume.rootDirCluster){
		parentdir = 0;
	}
	if(disk->fatType != 32 && parentdir<=1){
		parentdir = 0;
	}

	memclr(&direntry,sizeof(direntry));
	memcpy(&direntry,"..         ",11);
	direntry.attribute.flags.isDirectory = 1;
	_dirSetFirstClusterInDirEntry(&direntry,parentdir);
	memcpy(&dir[1],&direntry,sizeof(DIR_ENTRY));

	partition_releaseSector(disk,dir);


	// Now tell the FAT that the new directory is only one cluster long
	_fatSetNextClusterAddress(disk,nc,_fatGiveEndMarker(disk));

	return(0);
}

