#include "shader.h"

SDL_GPUShader* RG_LoadShader(
	SDL_GPUDevice* device,
	const char *shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	char *fullpath = NULL;
	
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return NULL;
	}

	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		fullpath = RG_GetAssetFile ("shaders/SPIRV/", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		
		SDL_Log ("Fuck Apple.");
		return NULL;
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		fullpath = RG_GetAssetFile ("shaders/DXIL/", shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return NULL;
	}

	size_t codeSize;
	void *code = SDL_LoadFile (fullpath, &codeSize);
	if (!code)
	{
		SDL_Log("Failed to load shader from disk! %s", fullpath);
		return NULL;
	}

	SDL_GPUShader* shader = SDL_CreateGPUShader (
		device,
		&(SDL_GPUShaderCreateInfo) {
		.code_size = codeSize,
		.code = (const Uint8 *)code,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_storage_textures = storageTextureCount,
		.num_storage_buffers = storageBufferCount,
		.num_uniform_buffers = uniformBufferCount,
	});
	if (shader == NULL)
	{
		SDL_Log ("Failed to create shader!");
		SDL_free (code);
		return NULL;
	}

	SDL_free (code);
	return shader;
}
