#include "gpu.h"

void
RG_GenerateBuffers(
	Context **context,
	Uint32 vertexBuffSize,
	Uint32 indexBuffSize,
	Uint32 textureBuffSize,
	Uint32 textureWidth,
	Uint32 textureHeight
)
{
	Uint32 win_w, win_h;
	
	(*context)->vertexBuffer = SDL_CreateGPUBuffer (
		(*context)->device,
		&(SDL_GPUBufferCreateInfo){
			.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = vertexBuffSize
		}
	);
	
	(*context)->indexBuffer = SDL_CreateGPUBuffer (
		(*context)->device,
		&(SDL_GPUBufferCreateInfo){
			.usage = SDL_GPU_BUFFERUSAGE_INDEX,
			.size = indexBuffSize
		}
	);
	(*context)->transferBuffer = SDL_CreateGPUTransferBuffer(
		(*context)->device,
		&(SDL_GPUTransferBufferCreateInfo){
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = vertexBuffSize + indexBuffSize
		}
	);	

	(*context)->textureBuffer = SDL_CreateGPUTexture (
		(*context)->device,
		&(SDL_GPUTextureCreateInfo){
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage =  SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = textureWidth,
			.height= textureHeight,
			.layer_count_or_depth = 1,
			.num_levels = 1
		}
	);
	
	SDL_GetWindowSize ((*context)->window, &win_w, &win_h);
	(*context)->depthBuffer = SDL_CreateGPUTexture (
		(*context)->device,
		&(SDL_GPUTextureCreateInfo){
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_D24_UNORM,
			.usage =  SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
			.width = win_w,
			.height= win_h,
			.layer_count_or_depth = 1,
			.num_levels = 1
		}
	);
	
	if ((*context)->textureBuffer == NULL)
	{
		SDL_Log ("ERROR CREATING TEXTURE BUFFER: %s", SDL_GetError ());
	}
	
	(*context)->texTransferBuffer = SDL_CreateGPUTransferBuffer(
		(*context)->device,
		&(SDL_GPUTransferBufferCreateInfo){
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = textureBuffSize
		}
	);	
}

void
RG_BindBuffers (
	SDL_GPURenderPass *renderPass,
	SDL_GPUBuffer *vertexBuff,
	SDL_GPUBuffer *indexBuff,
	SDL_GPUTexture *textureBuff,
	SDL_GPUSampler *sampler
)
{
	SDL_BindGPUVertexBuffers (renderPass, 0, &(SDL_GPUBufferBinding){
		.buffer = vertexBuff,
		.offset = 0
	}, 1);
	
	SDL_BindGPUIndexBuffer (renderPass, &(SDL_GPUBufferBinding){
		.buffer = indexBuff,
		.offset = 0
	}, SDL_GPU_INDEXELEMENTSIZE_16BIT);
	
	SDL_BindGPUFragmentSamplers (renderPass, 0, &(SDL_GPUTextureSamplerBinding){
		.texture = textureBuff,
		.sampler = sampler
	}, 1);
}

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
)
{
	//Upload vertice data
	SDL_UploadToGPUBuffer (
		copyPass,
		&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = transferBuffer,
			.offset = 0
		},
		&(SDL_GPUBufferRegion) {
			.buffer = vertexBuff,
			.offset = 0,
			.size = vertexBuffSize
		},
		false
	);
	
	//Upload index data
	SDL_UploadToGPUBuffer (
		copyPass,
		&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = transferBuffer,
			.offset = vertexBuffSize
		},
		&(SDL_GPUBufferRegion) {
			.buffer = indexBuff,
			.offset = 0,
			.size = indexBuffSize
		},
		false
	);
	
	//Upload texture data
	SDL_UploadToGPUTexture (
		copyPass,
		&(SDL_GPUTextureTransferInfo) {
			.transfer_buffer = texTransferBuffer,
			.offset = 0
		},
		&(SDL_GPUTextureRegion) {
			.texture = textureBuff,
			.w = textureWidth,
			.h = textureHeight,
			.d = 1
		},
		false
	);
}
