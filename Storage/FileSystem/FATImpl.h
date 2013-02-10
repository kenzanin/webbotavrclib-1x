/*
 * $Id: FATImpl.h,v 1.3 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATImpl.h,v $
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
 * FATImpl.h
 *
 *  Created on: 3 Mar 2010
 *      Author: Clive Webster
 *
 *  Define private methods not for use by applications
 */

#ifndef FATIMPL_H_
#define FATIMPL_H_

//#define FAT_DEBUG fat_debug_uart

#ifdef  FAT_DEBUG
#define RPRINTF_COMPLEX
#include "../../rprintf.h"
#include "../../uart.h"
extern HW_UART* FAT_DEBUG;
#endif

#include "FAT.h"
#include <string.h>

#define BUFFER_MODE_READONLY  1
#define BUFFER_MODE_READWRITE 2

// Directory attributes
#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME  	0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

#define memclr(dst,size) memset(dst,0,size)

void _ioReleaseSector(const DISK* disc,const void* buf);
static __inline__ void partition_releaseSector(const DISK* disk, const void *buf){
	_ioReleaseSector(disk,buf);
}
void* _partitionGetSector(const DISK* disk, SECTOR partSector, uint8_t mode);
boolean _partitionDirectSectorRead(const DISK *disk,SECTOR address, void* buf);
boolean _partitionDirectSectorWrite(const DISK *disk,SECTOR address, const void* buf);


boolean _fatNavigateTo(const DISK *disk, CLUSTER_NAV *nav,CLUSTER_COUNT relativeCluster);
CLUSTER _fatGiveEndMarker(const DISK* disk);
void _fatSetNextClusterAddress(const DISK* disk,CLUSTER cluster_addr,CLUSTER next_cluster_addr);
boolean  _fatExtend(DISK* disk,CLUSTER_NAV *nav,CLUSTER num_clusters);

CLUSTER _fatGetFreeCluster(DISK* disk);

SECTOR _diskClusterToSector(const DISK* disk,CLUSTER cluster);
void _dirSetFirstClusterInDirEntry(DIR_ENTRY *rec,CLUSTER cluster_addr);
int8_t _fileFindFile(const DISK* disk,const char* filename,DIR_POSITION *loc,CLUSTER *lastDir);
boolean _dirFindFreeFile(DISK* disk,const char* filename,DIR_POSITION *loc);
void _navInitClusterChain(CLUSTER_NAV *nav,CLUSTER cluster_addr);
void _partitionClearCluster(const DISK* disk,CLUSTER cluster);

boolean _dirGetFatFileName(const char* filename, char* fatfilename);
void _dirCreateDirectoryEntry(const DISK* disk,const DIR_ENTRY *filerec,const DIR_POSITION *loc);

#endif /* FATIMPL_H_ */
