
/*
 * $Id: FATdelfile.c,v 1.3 2010/06/14 19:16:07 clivewebster Exp $
 *
 * Revision History
 * ================
 * $Log: FATdelfile.c,v $
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
 * FaTdelfile.c
 *
 *  Created on: 4 Mar 2010
 *      Author: Clive Webster
 */

#include "FATImpl.h"

// Delete all cluster chains starting at the given entry
static void _fatUnlinkClusterChain(const DISK* disk,CLUSTER firstCluster){
	CLUSTER tbd=0;

	// Initialise to start of chain
	CLUSTER_NAV nav;
	_navInitClusterChain(&nav, firstCluster);

	CLUSTER_COUNT c=0;
	while(_fatNavigateTo(disk,&nav,c++)){
		if(tbd!=0){					// Delete any current cluster
			_fatSetNextClusterAddress(disk,tbd,0);
		}
		tbd=nav.currentCluster;		// The current cluser
	}
	_fatSetNextClusterAddress(disk,nav.currentCluster,0);
}



// Return value: TRUE if deleted

boolean fileDelete(DISK * disk,const char* filename){
	boolean rtn = FALSE;
	DIR_POSITION loc;

	if((_fileFindFile(disk,filename,&loc,0))==1){
		if(loc.attrib.flags.isReadOnly){
			return FALSE;
		}
		CLUSTER firstCluster;

		// Read the directory sector
		DIR_ENTRY* buf=_partitionGetSector(disk,loc.sector,BUFFER_MODE_READWRITE);
		DIR_ENTRY* dir = &buf[loc.entryInSector];
		firstCluster = dir->firstClusterHigh;
		firstCluster <<= 16;
		firstCluster |= dir->firstClusterLow;
		dir->filename[0] = 0xE5;		// Mark file as deleted
		partition_releaseSector(disk,buf);

 		_fatUnlinkClusterChain(disk,firstCluster);
		rtn = TRUE;
	}
	return rtn;
}

