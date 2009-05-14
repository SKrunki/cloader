#include <stdio.h>
#include <ogcsys.h>
#include <unistd.h>

#include "sys.h"
#include "wpad.h"
#include "video.h"

void Restart(void)
{
	printf("\n    Restarting Wii...");
	fflush(stdout);
	
	/* Load system menu */
	Sys_LoadMenu();
}

void Restart_Wait(void)
{
	printf("\n");
	sleep(3);
	printf("    Press any button to restart...");
	fflush(stdout);

	/* Wait for button */
	Wpad_WaitButtons();

	/* Restart */
	Restart();
}
 
