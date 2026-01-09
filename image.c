#include "image.h"

SDL_Surface *
LoadImage (const char* fn)
{
	SDL_Surface *result;
	SDL_PixelFormat format;

	result = SDL_LoadBMP (RG_GetAssetFile ("images", fn));
	
	if (!result)
	{
		SDL_Log ("Error loading image file %s: %s", fn, SDL_GetError ());
		return NULL;
	}
	
	if (result->format != SDL_PIXELFORMAT_ABGR8888)
	{
		SDL_Surface *next = SDL_ConvertSurface (
			result,
			SDL_PIXELFORMAT_ABGR8888
		);
		SDL_DestroySurface (result);
		result = next;
	}

	return result;
}
