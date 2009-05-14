#include <gctypes.h>
#include "disc.h"

extern int CONSOLE_XCOORD;
extern int CONSOLE_YCOORD;
extern int CONSOLE_WIDTH;
extern int CONSOLE_HEIGHT;
extern int CONSOLE_BG_COLOR;
extern int CONSOLE_FG_COLOR;

#define CFG_VIDEO_SYS   0  // system default
#define CFG_VIDEO_AUTO  1
#define CFG_VIDEO_GAME  1  // game default
#define CFG_VIDEO_PATCH 2  // patch mode
#define CFG_VIDEO_PAL50	3  // force PAL
#define CFG_VIDEO_PAL60	4  // force PAL60
#define CFG_VIDEO_NTSC	5  // force NTSC
#define CFG_VIDEO_VIDTV 6  // VIDTV patch
#define CFG_VIDEO_MAX   6

#define CFG_HOME_REBOOT 0
#define CFG_HOME_EXIT   1
#define CFG_HOME_SCRSHOT 2

//char languages[11][22] =
#define CFG_LANG_CONSOLE   0
#define CFG_LANG_JAPANESE  1
#define CFG_LANG_ENGLISH   2
#define CFG_LANG_GERMAN    3
#define CFG_LANG_FRENCH    4
#define CFG_LANG_SPANISH   5
#define CFG_LANG_ITALIAN   6
#define CFG_LANG_DUTCH     7
#define CFG_LANG_S_CHINESE 8
#define CFG_LANG_T_CHINESE 9
#define CFG_LANG_KOREAN   10
#define CFG_LANG_MAX      10

#define CFG_BTN_ORIGINAL  0
#define CFG_BTN_ULTIMATE  1
#define CFG_BTN_OPTIONS   2
#define CFG_BTN_OPTIONS_B 3

#define CFG_DEV_ASK  0
#define CFG_DEV_USB  1
#define CFG_DEV_SDHC 2

#define CFG_WIDE_NO   0
#define CFG_WIDE_YES  1
#define CFG_WIDE_AUTO 2

struct CFG
{
	int verbosemode;
	int video;
	int vidtv;
	int language;
	int ocarina;
	int ios;
};

extern struct CFG CFG;


