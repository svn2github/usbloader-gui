/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * demo.cpp
 * Basic template/demonstration of libwiigui capabilities. For a
 * full-featured app using many more extensions, check out Snes9x GX.
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "FreeTypeGX.h"
#include "video.h"
#include "audio.h"
#include "menu.h"
#include "input.h"
#include "filelist.h"
#include "main.h"
#include "http.h"
#include "dns.h"

#include "disc.h"
#include "wbfs.h"
#include "sys.h"
#include "video2.h"
#include "wpad.h"
#include "cfg.h"
#include <sdcard/wiisd_io.h>
#include <fat.h>


/* Constants */
#define CONSOLE_XCOORD		260
#define CONSOLE_YCOORD		115
#define CONSOLE_WIDTH		340
#define CONSOLE_HEIGHT		218

FreeTypeGX *fontSystem;
struct SSettings Settings;
//struct SSettings2 Settings2;
int ExitRequested = 0;
bool netcheck = false;

/*Networking - Forsaekn*/
int Net_Init(char *ip){

	s32 res;
    while ((res = net_init()) == -EAGAIN)
	{
		usleep(100 * 1000); //100ms
	}

    if (if_config(ip, NULL, NULL, true) < 0) {
		printf("      Error reading IP address, exiting");
		usleep(1000 * 1000 * 1); //1 sec
		return FALSE;
	}
	return TRUE;
}

void ExitApp()
{
	ShutoffRumble();
	StopGX();
	ShutdownAudio();
    //WPAD_Flush(0);
    //WPAD_Disconnect(0);
    //WPAD_Shutdown();
	//exit(0);
}

void
DefaultSettings()
{
	Settings.video = discdefault;
	Settings.vpatch = off;
	Settings.language = ConsoleLangDefault;
	Settings.ocarina = off;
	Settings.hddinfo = HDDInfo;
	Settings.sinfo = ((THEME.showID) ? GameID : Neither);
	Settings.rumble = RumbleOn;
	if (THEME.showRegion)
	{
		Settings.sinfo = ((Settings.sinfo == GameID) ? Both : GameRegion);
	}
	Settings.volume = v80;
}


int
main(int argc, char *argv[])
{
    __Disc_SetLowMem();
    s32 ret2;
    /* Load Custom IOS */
	ret2 = IOS_ReloadIOS(249);
	if (ret2 < 0) {
		printf("ERROR: cIOS could not be loaded!");
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

	//Sys_Init();
	//Video_SetMode();
	//Con_Init(CONSOLE_XCOORD, CONSOLE_YCOORD, CONSOLE_WIDTH, CONSOLE_HEIGHT);
	//Wpad_Init();

	__io_wiisd.startup();
	fatMountSimple("SD", &__io_wiisd);

	//load config file
	CFG_Load(argc, argv);

	//Init Network
/*	char myIP[16];
	if( !Net_Init(myIP) ){
		printf("Net_Init error");
		sleep(1);
		netcheck = false;
	}
	else netcheck = true;*/

    PAD_Init();
	InitVideo(); // Initialise video
	InitAudio(); // Initialize audio

	// read wiimote accelerometer and IR data
	//WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	//WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	// Initialize font system
	fontSystem = new FreeTypeGX();
	fontSystem->loadFont(font_ttf, font_ttf_size, 0);
	fontSystem->setCompatibilityMode(FTGX_COMPATIBILITY_DEFAULT_TEVOP_GX_PASSCLR | FTGX_COMPATIBILITY_DEFAULT_VTXDESC_GX_NONE);

	InitGUIThreads();
	DefaultSettings();
	MainMenu(MENU_CHECK);
	return 0;
}
