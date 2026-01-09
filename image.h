#ifndef _IMAGE_H
#define _IMAGE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include "init.h"
	
SDL_Surface *
LoadImage (const char* imageFilename);

#ifdef __cplusplus
}
#endif
#endif /* _IMAGE_H */
