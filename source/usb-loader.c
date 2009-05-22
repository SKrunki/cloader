#include <stdio.h>
#include <stdlib.h>
#include <ogcsys.h>
#include <unistd.h>
#include <string.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>

#include "disc.h"
#include "restart.h"
#include "sys.h"
#include "video.h"
#include "wbfs.h"
#include "wpad.h"
#include "cfg.h"
#include "fat.h"
#include "patchcode.h"

volatile u8 GameID[] = "CRAPPY";
volatile u8 ConfigChars[] = "CFGUSB1000000000";
const char* Config = (char*)ConfigChars+6;
s32 wbfsDev = WBFS_DEVICE_USB;
u32 USBIOS = 249;

char boottypes[2][15] = 
	{{"USB device"},
	{"SDHC card"}};

char videos[6][25] = 
	{{"Default"},
	{"Game default"},
	{"Force to console (auto)"},
	{"Force PAL50"},
	{"Force PAL60"},
	{"Force NTSC"}};

char languages[11][22] =
	{{"Console default"},
	{"Japanese"},
	{"English"},
	{"German"},
	{"French"},
	{"Spanish"},
	{"Italian"},
	{"Dutch"},
	{"S. Chinese"},
	{"T. Chinese"},
	{"Korean"}};

void printcursor(u8 item, u8 citem)
{
	printf("%s", (item == citem) ? "-> " : "   ");
}

void ConfigMenu()
{
	u8 CurrentItem = 0;
	s32 buttons = 0;
	CFG.verbosemode = 1;
	printf("\n*** Settings menu ***\n\n");
	do
	{
		printcursor(0, CurrentItem); printf("Boot type: %s               \n", boottypes[wbfsDev-1]);
		printcursor(1, CurrentItem); printf("Video mode: %s               \n", videos[CFG.video]);
		printcursor(2, CurrentItem); printf("VIDTV patch: %s               \n", CFG.vidtv ? "On" : "Off");
		printcursor(3, CurrentItem); printf("Ocarina: %s               \n", CFG.ocarina ? "On" : "Off");
		printcursor(4, CurrentItem); printf("Language: %s               \n", languages[CFG.language]);
		printf("\nUse arrows to change settings\nPress A to start game\n");
		buttons = Wpad_WaitButtons();
		if (buttons & WPAD_BUTTON_DOWN) CurrentItem = (CurrentItem+1) % 5;
		if (buttons & WPAD_BUTTON_UP) CurrentItem = CurrentItem ? (CurrentItem-1) : 4;
		if (buttons & WPAD_BUTTON_RIGHT)
		{
			switch (CurrentItem)
			{
				case 0: wbfsDev = (wbfsDev == WBFS_DEVICE_USB) ? WBFS_DEVICE_SDHC : WBFS_DEVICE_USB; break;
				case 1: CFG.video = (CFG.video+1) % 6; break;
				case 2: CFG.vidtv = !CFG.vidtv; break;
				case 3: CFG.ocarina = !CFG.ocarina; break;
				case 4: CFG.language = (CFG.language+1) % 11; break;
			}
		}
		if (buttons & WPAD_BUTTON_LEFT)
		{
			switch (CurrentItem)
			{
				case 0: wbfsDev = (wbfsDev == WBFS_DEVICE_USB) ? WBFS_DEVICE_SDHC : WBFS_DEVICE_USB; break;
				case 1: CFG.video = CFG.video ? CFG.video-1 : 5; break;
				case 2: CFG.vidtv = !CFG.vidtv; break;
				case 3: CFG.ocarina = !CFG.ocarina; break;
				case 4: CFG.language = CFG.language ? CFG.language-1 : 10; break;
			}
		}
		if (buttons & WPAD_BUTTON_HOME) Restart();
		if (!(buttons & WPAD_BUTTON_A)) printf("\x1b[8A");
	} while (!(buttons & WPAD_BUTTON_A));
	printf("\n");
}


int main(int argc, char **argv)
{
	s32 ret, game_num, buttons;
	u32 cnt, c;
	char game_title[31];
	struct discHdr *header_list = NULL;

	CFG.verbosemode = (Config[0] != '0' ? 1 : 0); // verbosemode output
	// Config byte #1 ignored
	// Config byte #2 (region) ignored
	CFG.ocarina = (Config[3] != '0' ? 1 : 0); // #3 Ocarina
	switch (Config[4]) // #4 videopatch
	{
		case '0': CFG.video = 0; break; // no patch
		case '1': CFG.video = 2; break; // force to console
		case '2': CFG.video = 1; break; // game's original videomode
		case '3': CFG.video = 3; break; // force PAL50
		case '4': CFG.video = 4; break; // force PAL60
		case '5': CFG.video= 5; break; // force NTSC
		default: CFG.video = 0; break;
	}	
	CFG.language = ((Config[5] >= '0') && (Config[5] <= '9')) ? Config[5] - '0' : 10; // #5 language
	wbfsDev = (Config[6] != '0') ? WBFS_DEVICE_SDHC : WBFS_DEVICE_USB; // #6 USB/SD
	CFG.vidtv = (Config[7] != '0' ? 1 : 0); // VIDTV Patch
	USBIOS = (Config[8] != '0' ? 222 : 249); // Use IOS222
	
	/* Load Custom IOS */
	ret = IOS_ReloadIOS(USBIOS);

	/* Initialize subsystems */
	Sys_Init();
	Wpad_Init();

	/* Set video mode */
	Video_SetMode();

	/* Initialize console */
	CONSOLE_XCOORD   = 60;
	CONSOLE_YCOORD   = 60;
	CONSOLE_WIDTH    = 500;
	CONSOLE_HEIGHT   = 402;

	Con_Init(CONSOLE_XCOORD, CONSOLE_YCOORD, CONSOLE_WIDTH, CONSOLE_HEIGHT);

	Con_Clear();
	if (CFG.verbosemode)
	{
		printf("cLoader v1.2 by Cluster\n\n");
		printf("* Hold B button to override default settings\n\n");
	}

	c = 0;
   do
	{		
		buttons = Wpad_GetButtons();
		c++;
	} while ((c < 1800000) && !(buttons & WPAD_BUTTON_B) && !(buttons & WPAD_BUTTON_1));
	if (buttons & WPAD_BUTTON_B) ConfigMenu();
	if (buttons & WPAD_BUTTON_1) CFG.verbosemode = 1;

	/* Check if Custom IOS is loaded */
	if (ret < 0) 
	{
		printf("[+] ERROR: Custom IOS could not be loaded! (ret = %d)\n", ret);
		Restart_Wait();
		return 0;
	}

	/* Initialize WBFS */
	if (CFG.verbosemode) printf("* Initializing WBFS...\n");
	ret = WBFS_Init(wbfsDev);
	if (ret < 0) 
	{
		int i;
		Wpad_Disconnect();
		for(i=30;i>=0;i--)
		{
			printf("\r* Waiting for device... %d ", i);
			IOS_ReloadIOS(USBIOS);
			sleep(1);
			ret = WBFS_Init(wbfsDev);
			if(ret>=0)
				break;
		}
		printf("\n");
		sleep(1);
	}

	if (ret < 0) 
	{
		Wpad_Init();
		printf("[+] ERROR: Could not initialize SD/USB subsystem! (ret = %d)\n", ret);
		Restart_Wait();
	}
	
	/* Initialize DIP module */
	ret = Disc_Init();
	if (ret < 0) {
		printf("[+] ERROR: Could not initialize DIP module! (ret = %d)\n", ret);
		Restart_Wait();
	}

	ret = WBFS_Open();
	if (ret < 0) {
		printf("[+] ERROR: No WBFS partition found!\n");
		printf("You need to format a partition and add some games!\n");
		Restart_Wait();
	}

	/* Get header list */
	if (CFG.verbosemode) printf("* Reading headers list...\n");
	ret = WBFS_GetCount(&cnt);
	if (ret < 0) {
		printf("[+] ERROR: Can't read games list!\n");
		Restart_Wait();
	}
	header_list = malloc(sizeof(struct discHdr) * cnt);
	memset(header_list, 0, sizeof(struct discHdr) * cnt);
	ret = WBFS_GetHeaders(header_list, cnt, sizeof(struct discHdr));
	if (ret < 0) {
		printf("[+] ERROR: Can't read header list!\n");
		Restart_Wait();
	}

	game_num = -1;
	for (c = 0; c < cnt; c++)
	{
		if (
				(header_list[c].id[0] == GameID[0]) &&
				(header_list[c].id[1] == GameID[1]) &&
				(header_list[c].id[2] == GameID[2]) &&
				(header_list[c].id[3] == GameID[3]) &&
				(header_list[c].id[4] == GameID[4]) &&
				(header_list[c].id[5] == GameID[5])
		) game_num = c;
	}

	if (game_num < 0) {
		printf("[+] ERROR: I can't find your game! Install it via WBFS manager first.\n");
		Restart_Wait();
	}

	if (CFG.verbosemode)
	{
		strncpy(game_title, header_list[game_num].title, 30);
		game_title[30] = 0;
		printf("\nTitle: %s\nID: %s\n\n", game_title, GameID);
	}

	/* Set WBFS mode */
	Disc_SetWBFS(wbfsDev, (u8*)GameID);

	/* Open disc */
	if (CFG.verbosemode) printf("* Opening disc...\n");
	ret = Disc_Open();
	if (ret < 0) {
		printf("[+] ERROR: Could not open game! (ret = %d)\n", ret);
		Restart_Wait();
	}

	if (CFG.verbosemode) printf("* Booting Wii game, please wait...\n");
	/* Boot Wii disc */
	Disc_WiiBoot();

	printf("[+] ERROR: Returned! Can't boot game!");;
	Restart_Wait();
	return 0;
}
