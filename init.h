#ifndef _INIT_H
#define _INIT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdbool.h>

typedef struct {
	SDL_Window *window;
	SDL_GPUDevice *device;
	MIX_Mixer *mixer;
	SDL_GPUGraphicsPipeline *pipeline;
	SDL_GPUBuffer *vertexBuffer, *indexBuffer;
	SDL_GPUTexture *textureBuffer;
	SDL_GPUTransferBuffer *transferBuffer, *texTransferBuffer;
	SDL_GPUSampler *sampler;
	float x, y, z;
} Context;

bool
RG_SetMetadata (
	const char *url,
	const char *by,
	const char *copy,
	const char *type,
	const char *name,
	const char *ver,
	const char *id
);

Context *
RG_Init (const char *gameName, int windowWidth, int windowHeight);

char *
RG_GetAssetFile (const char *subfolder, const char *filename);

char *
RG_GetSubFolder (const char *subfolder);

void
RG_Quit (Context *context);

#ifdef __cplusplus
}
#endif

#endif /* _INIT_H */
