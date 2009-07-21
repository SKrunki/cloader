/*
 *  Copyright (C) 2008 Nuke (wiinuke@gmail.com)
 *
 *  this file is part of GeckoOS for USB Gecko
 *  http://www.usbgecko.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <fat.h>
#include "ogc/ipc.h"
#include "fst.h"
#include "dvd_broadway.h"
#include "wpad.h"
#include "cfg.h"


#define FSTDIRTYPE 1
#define FSTFILETYPE 0
#define ENTRYSIZE 0xC
//#define FILEDIR	"fat0:/codes"
#define FILEDIR	"sd:/codes"

#define MAX_FILENAME_LEN	128


static vu32 dvddone = 0;


// Real basic 
u32 do_sd_code(char *filename)
{
	FILE *fp;
	u8 *filebuff;
	u32 filesize;
	u32 ret;
	char filepath[128];
	
	snprintf(filepath, sizeof(filepath), "%s/%s.gct", FILEDIR, filename);
	if (CFG.verbosemode) printf("* Ocarina: Loading %s\n",filepath);
	
	fp = fopen(filepath, "rb");
	if (!fp) {
		if (CFG.verbosemode) printf("[+] Ocarina: No SD codes found\n");
		sleep(2);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	filebuff = (u8*) malloc (filesize);
	if(filebuff == 0){
		fclose(fp);
		sleep(2);
		return 0;
	}

	ret = fread(filebuff, 1, filesize, fp);
	if(ret != filesize){	
		printf("[+] Ocarina: SD Code Error\n");
		free(filebuff);
		fclose(fp);
		sleep(2);
		return 0;
	}
   if (CFG.verbosemode) printf("* Ocarina: SD Codes found.\n");

	// ocarina config options are done elswhere, confirmation optional
	memcpy((void*)0x800027E8,filebuff,filesize);
	*(vu8*)0x80001807 = 0x01;

	free(filebuff);
	fclose(fp);
//	sleep(2);
	return 1;
}


