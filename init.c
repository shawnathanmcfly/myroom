#include "init.h"

bool
RG_SetMetadata (
	const char *url,
	const char *by,
	const char *copy,
	const char *type,
	const char *name,
	const char *ver,
	const char *id
)
{
	
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_URL_STRING, url))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
		
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_CREATOR_STRING, by))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
	
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_COPYRIGHT_STRING, copy))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
		
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_TYPE_STRING, type))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
		
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_NAME_STRING, name))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
		
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_VERSION_STRING, ver))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
		
	if (!SDL_SetAppMetadataProperty (SDL_PROP_APP_METADATA_IDENTIFIER_STRING, id))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return false;
	}
	
	return true;
}

Context *
RG_Init (
	const char *gameName,
	int windowWidth,
	int windowHeight
)
{
	Context *context = NULL;

	context = SDL_malloc (sizeof (Context));
	if (!context)
	{
		SDL_Log ("Failed to allocate memory for context.");
		return NULL;
	}
	
	context->mixer = NULL;
	context->pipeline = NULL;
	context->vertexBuffer = NULL;
	context->indexBuffer = NULL;
	context->textureBuffer = NULL;
	context->transferBuffer = NULL;
	context->sampler = NULL;
	context->z = -5.0f;
	context->y = 0.0f;
	context->x = 0.0f;
	
	if (!SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		SDL_Log ("RG ERROR: %s\n", SDL_GetError ());
		SDL_free (context);
		context = NULL;
		return NULL;
	}
	
	/*
	 * 
	 * 	Init Audio Stuff
	 * 
	 * 
	 */
	if (!MIX_Init ())
		SDL_Log ("ERROR: %s", SDL_GetError ());
		
	context->window = SDL_CreateWindow (
		gameName,
		windowWidth,
		windowHeight,
		0
	);
	
	if (!context->window)
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		SDL_free (context);
		context = NULL;
		return NULL;
	}
	
	SDL_Surface *logo = SDL_LoadBMP (RG_GetAssetFile ("images", "logo.bmp"));
	SDL_SetWindowIcon (context->window, logo);
	SDL_DestroySurface (logo);
		
	context->device = SDL_CreateGPUDevice (	
		SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL,
		true,
		NULL
	);
	
	if (!context->device)
	{
		SDL_DestroyWindow (context->window);
		context->window = NULL;
		SDL_free (context);
		context = NULL;
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return NULL;
	}
	
	if (!SDL_ClaimWindowForGPUDevice (context->device, context->window))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		SDL_DestroyGPUDevice (context->device);
		SDL_DestroyWindow (context->window);
		context->window = NULL;
		context->device = NULL;
		SDL_free (context);
		context = NULL;
	}
	
	return context;
}

void
RG_Quit (Context *context)
{
	if (context)
	{
		
		SDL_ReleaseGPUGraphicsPipeline (context->device, context->pipeline);
		SDL_ReleaseGPUBuffer(context->device, context->vertexBuffer);
		SDL_ReleaseGPUBuffer(context->device, context->indexBuffer);
		SDL_ReleaseGPUTexture(context->device, context->textureBuffer);
		SDL_ReleaseGPUSampler (context->device, context->sampler);
		
		if (context->device && context->window)
			SDL_ReleaseWindowFromGPUDevice (context->device, context->window);
			
		if (context->window)	
			SDL_DestroyWindow (context->window);
			
		if (context->device)
			SDL_DestroyGPUDevice (context->device);
			
		if (context->mixer)
			MIX_DestroyMixer (context->mixer);
		
		context->window = NULL;
		context->device = NULL;
		context->mixer = NULL;
		
		SDL_free (context);
		context = NULL;
	}
}

char *
RG_GetAssetFile (const char *subfolder, const char *filename)
{
	static char absPath[2000];
	SDL_snprintf (
		absPath,
		sizeof(absPath),
		"%s%s/%s",
		SDL_GetBasePath (),
		subfolder,
		filename
	);
	
	return absPath;
}

char *
RG_GetSubFolder (const char *subfolder)
{
	static char absPath[1000];
	SDL_snprintf (
		absPath,
		sizeof(absPath),
		"%s%s",
		SDL_GetBasePath (),
		subfolder
	);
	
	return absPath;
}
