#ifndef _SHADER_H
#define _SHADER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include "init.h"

SDL_GPUShader* RG_LoadShader(
	SDL_GPUDevice* device,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
);

#ifdef __cplusplus
}
#endif
#endif /* _SHADER_H */
