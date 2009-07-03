/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken at libsdl.or
*/
#include "SDL_config.h"

// Public includes.
#include "SDL_timer.h"

// Audio internal includes.
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_sysaudio.h"
#include "../SDL_audio_c.h"

// GameCube audio internal includes.
#include <ogcsys.h>
#include <ogc/audio.h>
#include <ogc/cache.h>
#include "SDL_gamecubeaudio.h"

#define SAMPLES_PER_DMA_BUFFER (512)

static const char	GAMECUBEAUD_DRIVER_NAME[] = "gamecube";
static Uint32 dma_buffers[2][SAMPLES_PER_DMA_BUFFER*8] __attribute__((aligned(32)));
static Uint8 whichab = 0;
#define AUDIOSTACK 16384*2
static lwpq_t audioqueue;
static lwp_t athread;
static Uint8 astack[AUDIOSTACK];

/****************************************************************************
 * Audio Threading
 ***************************************************************************/
static void *
AudioThread (void *arg)
{
	LWP_InitQueue (&audioqueue);

	while (1)
	{
		whichab ^= 1;
		memset(dma_buffers[whichab], 0, sizeof(dma_buffers[0]));

		// Is the device ready?
		if (current_audio && (!current_audio->paused))
		{
			// Is conversion required?
			if (current_audio->convert.needed)
			{
				// Dump out the conversion info.
				// printf("----\n");
				// printf("conversion is needed\n");
				// printf("\tsrc_format = 0x%x\n", current_audio->convert.src_format);
				// printf("\tdst_format = 0x%x\n", current_audio->convert.dst_format);
				// printf("\trate_incr  = %f\n", (float) current_audio->convert.rate_incr);
				// printf("\tbuf        = 0x%08x\n", current_audio->convert.buf);
				// printf("\tlen        = %d\n", current_audio->convert.len);
				// printf("\tlen_cvt    = %d\n", current_audio->convert.len_cvt);
				// printf("\tlen_mult   = %d\n", current_audio->convert.len_mult);
				// printf("\tlen_ratio  = %f\n", (float) current_audio->convert.len_ratio);

				//SDL_mutexP(current_audio->mixer_lock);
				// Get the client to produce audio.
				current_audio->spec.callback(
					current_audio->spec.userdata,
					current_audio->convert.buf,
					current_audio->convert.len);
				//SDL_mutexV(current_audio->mixer_lock);

				// Convert the audio.
				SDL_ConvertAudio(&current_audio->convert);

				// Sanity check.
				//if (sizeof(DMABuffer) != current_audio->convert.len_cvt)
				//{
						//printf("The size of the DMA buffer (%u) doesn't match the converted buffer (%u)\n",
						//sizeof(DMABuffer), current_audio->convert.len_cvt);
				//}

				// Copy from SDL buffer to DMA buffer.
				memcpy(dma_buffers[whichab], current_audio->convert.buf, current_audio->convert.len_cvt);
			}
			else
			{
				//printf("conversion is not needed\n");
				//SDL_mutexP(current_audio->mixer_lock);
				current_audio->spec.callback(
					current_audio->spec.userdata,
					(Uint8 *)dma_buffers[whichab],
					SAMPLES_PER_DMA_BUFFER*4);
				//SDL_mutexV(current_audio->mixer_lock);
			}
		}
		LWP_ThreadSleep (audioqueue);
	}

	return NULL;
}

/****************************************************************************
 * MixSamples
 * This continually calls S9xMixSamples On each DMA Completion
 ***************************************************************************/
static void
DMACallback()
{
	AUDIO_StopDMA ();
	DCFlushRange (dma_buffers[whichab], sizeof(dma_buffers[0]));
	AUDIO_InitDMA ((Uint32)dma_buffers[whichab], SAMPLES_PER_DMA_BUFFER*4);
	AUDIO_StartDMA ();

	LWP_ThreadSignal (audioqueue);
}

static int GAMECUBEAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	if (spec->freq != 32000 && spec->freq != 48000)
		spec->freq = 32000;

	// Set up actual spec.
	spec->format	= AUDIO_S16MSB;
	spec->channels	= 2;
	spec->samples	= SAMPLES_PER_DMA_BUFFER;
	spec->padding	= 0;
	SDL_CalculateAudioSpec(spec);

	memset(dma_buffers[0], 0, sizeof(dma_buffers[0]));
	memset(dma_buffers[1], 0, sizeof(dma_buffers[0]));

	if (spec->freq == 32000)
		AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
	else
		AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);

	// startup conversion thread
	LWP_CreateThread (&athread, AudioThread, NULL, astack, AUDIOSTACK, 65);

	// Start the first chunk of audio playing
	DMACallback();

	return 1;
}

void static GAMECUBEAUD_WaitAudio(_THIS)
{

}

static void GAMECUBEAUD_PlayAudio(_THIS)
{

}

static Uint8 *GAMECUBEAUD_GetAudioBuf(_THIS)
{
	return NULL;
}


static void GAMECUBEAUD_CloseAudio(_THIS)
{
	// Stop any DMA going on
	AUDIO_StopDMA();

	// terminate conversion thread
	LWP_ThreadSignal(audioqueue);
}

static void GAMECUBEAUD_DeleteDevice(SDL_AudioDevice *device)
{
	// Forget the DMA callback
	AUDIO_RegisterDMACallback(0);

	// Stop any DMA going on
	AUDIO_StopDMA();

	// terminate conversion thread
	LWP_ThreadSignal(audioqueue);

	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *GAMECUBEAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	// Initialise the Wii side of the audio system
	AUDIO_Init(0);
	AUDIO_RegisterDMACallback(DMACallback);

	/* Set the function pointers */
	this->OpenAudio = GAMECUBEAUD_OpenAudio;
	this->WaitAudio = GAMECUBEAUD_WaitAudio;
	this->PlayAudio = GAMECUBEAUD_PlayAudio;
	this->GetAudioBuf = GAMECUBEAUD_GetAudioBuf;
	this->CloseAudio = GAMECUBEAUD_CloseAudio;
	this->free = GAMECUBEAUD_DeleteDevice;

	return this;
}

static int GAMECUBEAUD_Available(void)
{
	return 1;
}

AudioBootStrap GAMECUBEAUD_bootstrap = {
	GAMECUBEAUD_DRIVER_NAME, "SDL GameCube audio driver",
	GAMECUBEAUD_Available, GAMECUBEAUD_CreateDevice
};
