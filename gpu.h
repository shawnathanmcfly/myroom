#ifndef _GPU_H
#define _GPU_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <stdbool.h>

#include "init.h"
#include "image.h"

void
RG_GenerateBuffers(
	Context **context,
	Uint32 vertexBuffSize,
	Uint32 indexBuffSize,
	Uint32 textureBuffSize,
	Uint32 textureWidth,
	Uint32 textureHeight
);

void
RG_BindBuffers (
	SDL_GPURenderPass *renderPass,
	SDL_GPUBuffer *vertexBuff,
	SDL_GPUBuffer *indexBuff,
	SDL_GPUTexture *textureBuff,
	SDL_GPUSampler *sampler
);

void
RG_UploadToGPUBuffer (
	SDL_GPUCopyPass *copyPass,
	SDL_GPUTransferBuffer *transferBuffer,
	SDL_GPUTransferBuffer *texTransferBuffer,
	SDL_GPUBuffer *vertexBuff,
	SDL_GPUBuffer *indexBuff,
	SDL_GPUTexture *textureBuff,
	Uint32 textureWidth,
	Uint32 textureHeight,
	Uint32 vertexBuffSize,
	Uint32 indexBuffSize
);

#ifdef __cplusplus
}
#endif
#endif /* _GPU_H */
