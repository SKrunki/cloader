#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ogcsys.h>
#include <fat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sdcard/wiisd_io.h>
#include "wbfs.h"

/* Constants */
//#define SDHC_MOUNT	"sdhc"
#define SDHC_MOUNT	"sd"

/* Disc interfaces */
extern const DISC_INTERFACE __io_sdhc;

extern s32 wbfsDev;

#define MOUNT_NONE 0
#define MOUNT_SD   1
#define MOUNT_SDHC 2

static int sd_mount_mode = MOUNT_NONE;

s32 Fat_MountSDHC(void)
{
	s32 ret;

	sd_mount_mode = MOUNT_NONE;

	/* Initialize SDHC interface */
	ret = __io_sdhc.startup();
	if (!ret) {
		//printf("[+] ERROR: SDHC init! (%d)\n", ret); sleep(1);
		goto try_sd;
		//return -1;
	}

	/* Mount device */
	ret = fatMountSimple(SDHC_MOUNT, &__io_sdhc);
	if (!ret) {
		//printf("[+] ERROR: SDHC/FAT init! (%d)\n", ret); sleep(1);
		return -2;
	}

	sd_mount_mode = MOUNT_SDHC;
	return 0;

	try_sd:
	// SDHC failed, try SD
	ret = __io_wiisd.startup();
	if (!ret) {
		//printf("[+] ERROR: SD init! (%d)\n", ret); sleep(1);
		return -3;
	}
	ret = fatMountSimple("sd", &__io_wiisd);
	if (!ret) {
		// printf("[+] ERROR: SD/FAT init! (%d)\n", ret); sleep(1);
		return -4;
	}
	//printf("[+] NOTE: SDHC mode not available\n");
	//printf("[+] NOTE: card in standard SD mode\n\n");

	sd_mount_mode = MOUNT_SD;
	return 0;
}

s32 Fat_UnmountSDHC(void)
{
	s32 ret = 1;

	/* Unmount device */
	fatUnmount(SDHC_MOUNT);

	/* Shutdown SDHC interface */
	if (sd_mount_mode == MOUNT_SDHC) {
		// don't shutdown sdhc if we're booting from it
		if (wbfsDev != WBFS_DEVICE_SDHC) {
			ret = __io_sdhc.shutdown();
		}
	} else if (sd_mount_mode == MOUNT_SD) {
		ret = __io_wiisd.shutdown();
	}
	sd_mount_mode = MOUNT_NONE;
	if (!ret)
		return -1;

	return 0;
}

s32 Fat_ReadFile(const char *filepath, void **outbuf)
{
	FILE *fp     = NULL;
	void *buffer = NULL;

	struct stat filestat;
	u32         filelen;

	s32 ret;

	/* Get filestats */
	stat(filepath, &filestat);

	/* Get filesize */
	filelen = filestat.st_size;

	/* Allocate memory */
	buffer = memalign(32, filelen);
	if (!buffer)
		goto err;

	/* Open file */
	fp = fopen(filepath, "rb");
	if (!fp)
		goto err;

	/* Read file */
	ret = fread(buffer, 1, filelen, fp);
	if (ret != filelen)
		goto err;

	/* Set pointer */
	*outbuf = buffer;

	goto out;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	/* Error code */
	ret = -1;

out:
	/* Close file */
	if (fp)
		fclose(fp);

	return ret;
}
