#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <ogcsys.h>

#include "cfg.h"
#include "wpad.h"

int CONSOLE_XCOORD   = 258;
int CONSOLE_YCOORD   = 112;
int CONSOLE_WIDTH    = 354;
int CONSOLE_HEIGHT   = 304;

int CONSOLE_FG_COLOR = 15;
int CONSOLE_BG_COLOR = 0;

struct CFG CFG;

