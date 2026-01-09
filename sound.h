#ifndef _SOUND_H
#define _SOUND_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "init.h"
	
bool
RG_InitAudio (Context **context);

bool
RG_PlaySong (Context *context, const char* filename);

#ifdef __cplusplus
}
#endif
#endif /* _SOUND_H */
