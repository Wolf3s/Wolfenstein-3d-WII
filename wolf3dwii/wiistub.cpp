/* ================================================
   Wolfenstein 3D (SDL) Wii
   Based on Wolf4SDL v.1.6
   Ported to Wii by Nick [MrPeanut] Cavallo
   ================================================

   ================================================
   file: wiistub.cpp author: MrPeanut 
   purpose: all things wii!
   ================================================
*/

#include "wl_def.h"
#include "wiistub.h"

#include <gccore.h>
#include <fat.h>
#include <wiiuse/wpad.h>
#include <unistd.h>
#include <debug.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

FILE *wiilog = NULL;

#ifdef __cplusplus
extern "C"           // we need C style linkage for SDL_Main
{
	int main (int argc, char **argv)
	{
		char *curdir;
		int i;

	//VIDEO_Init();
	//rmode = VIDEO_GetPreferredMode(NULL);
	//xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	//console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	//VIDEO_Configure(rmode);
	//VIDEO_SetNextFramebuffer(xfb);
	//VIDEO_SetBlack(FALSE);
	//VIDEO_Flush();

	//VIDEO_WaitVSync();
	//if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	//DEBUG_Init(GDBSTUB_DEVICE_WIFI, 8000); // Port 8000 (use whatever you want)
	//_break();
		
	printf("\n\n\n\n\n");
	
	printf(" =====================================================\n");
	printf(" Wolfenstein 3D Wii v.1.3 by Nick [MrPeanut] Cavallo\n");
	printf(" Based on Wolf4SDL v.1.6\n");
	printf(" =====================================================\n\n");

	if (!fatInitDefault()) {
		Quit("fatInitDefault: failed");
	}

	if (WPAD_Init() != 0) {
		Quit("WPAD_Init: failed");
	}

	

	curdir = argv[0];

	for(i = strlen(curdir); i >= 0; i--)
	{
		if (curdir[i] == '/')
		{
			curdir[i+1] = '\0';
			break;
		}
	}

	chdir(curdir);

	printf("%s\n", curdir);

	game_init( argc, argv );
	return(1);
	
	
	}
}
#endif

int wiiKbHit(void)
{
	WPAD_ScanPads();

	return WPAD_ButtonsDown(0);
}

short wiiShortSwap (short l)
{
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

int wiiLongSwap (int l)
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}


void wiiSleepUntilKbHit(void)
{
	while (wiiKbHit() == 0)
		;;
}

void wiiProcessFont(int chunk)
{
	fontstruct  *font;

	font = (fontstruct *) grsegs[chunk];

	font->height = wiiShortSwap(font->height);
	

	for (int i = 0; i < 256; i++)
		font->location[i] = wiiShortSwap(font->location[i]);

}





	






