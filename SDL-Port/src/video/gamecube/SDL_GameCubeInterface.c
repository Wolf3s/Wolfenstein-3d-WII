/* SDL_GameCubeInterface.c
=================================================================
04/27/09 MrPeanut
This file deals with input using different devices specifically for wolf3d
Read SDL_GameCubeInterface.h for more information
*/

#include "SDL_config.h"

#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include <wiiuse/wpad.h>

#include "SDL_gamecubevideo.h"
#include "SDL_gamecubeevents_c.h"
#include <SDL/SDL_GamecubeInterface.h>


// Globals for last keystate

Uint8 lastButtonStateA = SDL_RELEASED;
Uint8 lastButtonStateB = SDL_RELEASED;
Uint8 lastButtonStateZ = SDL_RELEASED;
Uint8 lastButtonStateHome = SDL_RELEASED; // start on gamecube pad
Uint8 lastButtonState1 = SDL_RELEASED; // x on gamecube pad
Uint8 lastButtonState2 = SDL_RELEASED;  // y on gamecube pad
Uint8 lastButtonStatePlus = SDL_RELEASED; // c right on gamecube pad
Uint8 lastButtonStateMinus = SDL_RELEASED; // c left on gamecube pad
Uint8 lastButtonStateUp = SDL_RELEASED;
Uint8 lastButtonStateDown = SDL_RELEASED;
Uint8 lastButtonStateLeft = SDL_RELEASED;
Uint8 lastButtonStateRight = SDL_RELEASED;
Uint8 lastButtonStateL = SDL_RELEASED;
Uint8 lastButtonStateR = SDL_RELEASED;

int control_mode;
int i;
int offscreen = 0;
int lastX;

int GCI_ChangeInterface(int mode)
{
	PADStatus pad_status[PAD_CHANMAX];
	PAD_Sync();
	
	if( mode == CONTROL_GAMECUBE )
	{
		PAD_Read(pad_status);
		if (pad_status[0].err != PAD_ERR_NONE)
			return CONTROL_ERROR;
		
		WPAD_Disconnect(WPAD_CHAN_ALL);  // turn off wiimotes if gamecube mode is selected
	}
	
	control_mode = mode;
	return mode;
}

void ModernFrame(void)
{
	struct expansion_t data;
	
	// Wiimote
	
	Uint8 stateUp = SDL_RELEASED;
	Uint8 stateDown = SDL_RELEASED;
	Uint8 stateLeft = SDL_RELEASED;
	Uint8 stateRight = SDL_RELEASED;
	Uint8 stateA = SDL_RELEASED;
	Uint8 statePlus = SDL_RELEASED;
	Uint8 stateMinus = SDL_RELEASED;
	Uint8 stateHome = SDL_RELEASED;
	Uint8 stateB = SDL_RELEASED;

	// Nunchuk

	Uint8 stateZ = SDL_RELEASED;
	Uint8 state1 = SDL_RELEASED; // c button
	Uint8 stateL = SDL_RELEASED; // x axis joystick
	Uint8 stateR = SDL_RELEASED; // x axis joystick

	WPAD_ScanPads();							
    WPADData *wd = WPAD_Data(0);
	
	// Wiimote

	// x axis movement (turning)

	if( offscreen == 1 && wd->ir.raw_valid )  // if the last frame is off but it is now on screen
		offscreen = 0;

	if( offscreen == 0 && !wd->ir.raw_valid )  
	{
		offscreen = 1;  // update last frame
		SDL_PrivateMouseMotion( 0, 0, 160, 100 );  // center the mouse (to make you stop moving)
	}

	if( offscreen == 0 )  // everything else is good, update your x
	{
		if( lastX != wd->ir.x )
		{
			SDL_PrivateMouseMotion( 0, 0, wd->ir.x, 100 );
			lastX = wd->ir.x;
		}
	}
	  
	// nunchuk x axis movement, y is handled thru the joystick interface

	/*

	if( wd->exp.type == WPAD_EXP_NUNCHUK )
	{
		if( wd->exp.nunchuk.js.pos.x - wd->exp.nunchuk.js.center.x > 20 )
			stateR = SDL_PRESSED;

		if( wd->exp.nunchuk.js.pos.x - wd->exp.nunchuk.js.center.x < -20 )
			stateL = SDL_PRESSED;
	}

	*/



	if(wd->btns_h & WPAD_BUTTON_UP)
	{
		stateUp = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_DOWN)
	{
		stateDown = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_LEFT)
	{
		stateLeft = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_RIGHT)
	{
		stateRight = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_PLUS)
	{
		statePlus = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_MINUS)
	{
		stateMinus = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_HOME)
	{
		stateHome = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_A)
	{
		stateA = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_B)
	{
		stateB = SDL_PRESSED;
	}
	
	// nunchuck
	
	if(wd->btns_h & WPAD_NUNCHUK_BUTTON_Z)
	{
		stateZ = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_NUNCHUK_BUTTON_C)
	{
		state1 = SDL_PRESSED;
	}

	//if(stateL != lastButtonStateL)
	//{
	//	lastButtonStateL = stateL;
	//	SDL_keysym keysym;
	//	SDL_memset(&keysym, 0, (sizeof keysym));
	//	keysym.sym = SDLK_z;
	//	SDL_PrivateKeyboard(stateL, &keysym);
	//}

	//if(stateR != lastButtonStateR)
	//{
	//	lastButtonStateR = stateR;
	//	SDL_keysym keysym;
	//	SDL_memset(&keysym, 0, (sizeof keysym));
	//	keysym.sym = SDLK_x;
	//	SDL_PrivateKeyboard(stateR, &keysym);
	//}

	if(stateUp != lastButtonStateUp)
	{
		lastButtonStateUp = stateUp;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_UP;
		SDL_PrivateKeyboard(stateUp, &keysym);
	}
	if(stateDown != lastButtonStateDown)
	{
		lastButtonStateDown = stateDown;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_DOWN;
		SDL_PrivateKeyboard(stateDown, &keysym);
	}
	if(stateLeft != lastButtonStateLeft)
	{
		lastButtonStateLeft = stateLeft;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LEFT;
		SDL_PrivateKeyboard(stateLeft, &keysym);
	}
	if(stateRight != lastButtonStateRight)
	{
		lastButtonStateRight = stateRight;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_RIGHT;
		SDL_PrivateKeyboard(stateRight, &keysym);
	}
	if(statePlus != lastButtonStatePlus)
	{
		lastButtonStatePlus = statePlus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(statePlus, &keysym);
	}
	if(stateMinus != lastButtonStateMinus)
	{
		lastButtonStateMinus = stateMinus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(stateMinus, &keysym);
	}
	if(stateA != lastButtonStateA)
	{
		lastButtonStateA = stateA;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_SPACE;
		SDL_PrivateKeyboard(stateA, &keysym);
	}
	if(stateB != lastButtonStateB)
	{
		lastButtonStateB = stateB;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LCTRL;
		SDL_PrivateKeyboard(stateB, &keysym);
	}
	if(stateZ != lastButtonStateZ)
	{
		lastButtonStateZ = stateZ;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LSHIFT;
		SDL_PrivateKeyboard(stateZ, &keysym);
	}
	if(state1 != lastButtonState1)  // c button on nunchuk
	{
		lastButtonState1 = state1;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_SPACE;
		SDL_PrivateKeyboard(state1, &keysym);
	}
}

void GamecubePadFrame(void)
{
	Uint8 stateR = SDL_RELEASED;
	Uint8 stateL = SDL_RELEASED;
	Uint8 stateZ = SDL_RELEASED;
	Uint8 state2 = SDL_RELEASED;
	Uint8 state1 = SDL_RELEASED;
	Uint8 stateMinus = SDL_RELEASED;
	Uint8 stateHome = SDL_RELEASED;
	Uint8 statePlus = SDL_RELEASED;
	Uint8 stateUp = SDL_RELEASED;
	Uint8 stateDown = SDL_RELEASED;
	Uint8 stateLeft = SDL_RELEASED;
	Uint8 stateRight = SDL_RELEASED;
	Uint8 stateA = SDL_RELEASED;
	Uint8 stateB = SDL_RELEASED;
	Uint16 buttonsHeld;
	
	PAD_ScanPads();
	
	buttonsHeld = PAD_ButtonsHeld(0);

	//if(PAD_SubStickX(0) > 18)
	//{
	//	statePlus = SDL_PRESSED; // plus is c right
	//}
	//if(PAD_SubStickX(0) < -18)
	//{
	//	stateMinus = SDL_PRESSED; // minus is c left
	//}
	if(buttonsHeld & PAD_BUTTON_A)
	{
		stateA = SDL_PRESSED;
	}
	if(buttonsHeld & PAD_BUTTON_B)
	{
		stateB = SDL_PRESSED;
	}
	if(buttonsHeld & PAD_BUTTON_START)
	{
		stateHome = SDL_PRESSED; // start button is home
	}
	if(buttonsHeld & PAD_BUTTON_X)
	{
		state1 = SDL_PRESSED; // 1 is X
	}
	if(buttonsHeld & PAD_BUTTON_Y)
	{
		state2 = SDL_PRESSED; // 2 is y
	}
	if(buttonsHeld & PAD_TRIGGER_Z)
	{
		stateZ = SDL_PRESSED;
	}
	if(buttonsHeld & PAD_TRIGGER_L)
	{
		stateL = SDL_PRESSED;
	}
	if(buttonsHeld & PAD_TRIGGER_R)
	{
		stateR = SDL_PRESSED;
	}
	if(stateB != lastButtonStateB)
	{
		lastButtonStateB = stateB;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_SPACE;
		SDL_PrivateKeyboard(stateB, &keysym);
	}
	if(stateA != lastButtonStateA)
	{
		lastButtonStateA = stateA;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LCTRL;
		SDL_PrivateKeyboard(stateA, &keysym);
	}
	if(statePlus != lastButtonStatePlus)
	{
		lastButtonStatePlus = statePlus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_x;
		SDL_PrivateKeyboard(statePlus, &keysym);
	}
	if(stateMinus != lastButtonStateMinus)
	{
		lastButtonStateMinus = stateMinus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_z;
		SDL_PrivateKeyboard(stateMinus, &keysym);
	}
	if(stateHome != lastButtonStateHome)
	{
		lastButtonStateHome = stateHome;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(stateHome, &keysym);
	}
	if(state1 != lastButtonState1)
	{
		lastButtonState1 = state1;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_RETURN;
		SDL_PrivateKeyboard(state1, &keysym);
	}
	if(state2 != lastButtonState2)
	{
		lastButtonState2 = state2;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_RETURN;
		SDL_PrivateKeyboard(state2, &keysym);
	}
	if(stateZ != lastButtonStateZ)
	{
		lastButtonStateZ = stateZ;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LSHIFT;
		SDL_PrivateKeyboard(stateZ, &keysym);
	}
	if(stateL != lastButtonStateL)
	{
		lastButtonStateL = stateL;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_z;
		SDL_PrivateKeyboard(stateL, &keysym);
	}
	if(stateR != lastButtonStateR)
	{
		lastButtonStateR = stateR;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_x;
		SDL_PrivateKeyboard(stateR, &keysym);
	}
}

void WiimoteFrame()
{
	WPAD_ScanPads();							
    WPADData *wd = WPAD_Data(0);

	Uint8 stateA = SDL_RELEASED;
	Uint8 stateB = SDL_RELEASED;
	Uint8 state2 = SDL_RELEASED;
	Uint8 state1 = SDL_RELEASED;
	Uint8 statePlus = SDL_RELEASED;
	Uint8 stateMinus = SDL_RELEASED;
	Uint8 stateUp = SDL_RELEASED;
	Uint8 stateDown = SDL_RELEASED;
	Uint8 stateLeft = SDL_RELEASED;
	Uint8 stateRight = SDL_RELEASED;
	Uint8 stateHome = SDL_RELEASED;

	if(wd->btns_h & WPAD_BUTTON_B)
	{
		stateB = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_A)
	{
		stateA = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_2)
	{
		state2 = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_1)
	{
		state1 = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_PLUS)
	{
		statePlus = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_MINUS)
	{
		stateMinus = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_UP)
	{
		stateUp = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_DOWN)
	{
		stateDown = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_LEFT)
	{
		stateLeft = SDL_PRESSED;
	}
	if(wd->btns_h & WPAD_BUTTON_RIGHT)
	{
		stateRight = SDL_PRESSED;
	}
	if(stateB != lastButtonStateB)
	{
		lastButtonStateB = stateB;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LALT;
		SDL_PrivateKeyboard(stateB, &keysym);
	}
	if(stateA != lastButtonStateA)
	{
		lastButtonStateA = stateA;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_RETURN;
		SDL_PrivateKeyboard(stateA, &keysym);
	}
	if(state2 != lastButtonState2)
	{
		lastButtonState2 = state2;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LCTRL;
		SDL_PrivateKeyboard(state2, &keysym);
	}
	if(state1 != lastButtonState1)
	{
		lastButtonState1 = state1;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_SPACE;
		SDL_PrivateKeyboard(state1, &keysym);
	}
	if(statePlus != lastButtonStatePlus)
	{
		lastButtonStatePlus = statePlus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(statePlus, &keysym);
	}
	if(stateMinus != lastButtonStateMinus)
	{
		lastButtonStateMinus = stateMinus;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(stateMinus, &keysym);
	}
	if(stateUp != lastButtonStateUp)
	{
		lastButtonStateUp = stateUp;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_LEFT;
		SDL_PrivateKeyboard(stateUp, &keysym);
	}
	if(stateDown != lastButtonStateDown)
	{
		lastButtonStateDown = stateDown;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_RIGHT;
		SDL_PrivateKeyboard(stateDown, &keysym);
	}
	if(stateLeft != lastButtonStateLeft)
	{
		lastButtonStateLeft = stateLeft;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_DOWN;
		SDL_PrivateKeyboard(stateLeft, &keysym);
	}
	if(stateRight != lastButtonStateRight)
	{
		lastButtonStateRight = stateRight;
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_UP;
		SDL_PrivateKeyboard(stateRight, &keysym);
	}
	if(wd->btns_h & WPAD_BUTTON_HOME)
	{
		stateHome = SDL_PRESSED;
	}
	/*if(stateHome != lastButtonStateHome)  // This quits the app when the home button is pressed, sometime I'd like to figure out how to display the "home" menu...
	{
		lastButtonStateHome = stateHome;
	
		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}
	*/
}

