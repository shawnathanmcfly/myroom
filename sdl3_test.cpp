#define SDL_MAIN_USE_CALLBACKS 1
#define CGLTF_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <math.h>
#include "init.h"
#include "image.h"
#include "shader.h"
#include "gpu.h"
#include "sound.h"
#include "action.h"
#include "vertex.h"
#include "glb.h"

#define GAME_NAME "My Room"

using namespace std;

typedef struct
{
	mat4s mvp;
} UBO;

SDL_AppResult
SDL_AppInit (void **appstate, int argc, char *argv[])
{
	Context *context = nullptr;
	Actions *actions = nullptr;
	vector<Action> action_list;
	Uint32 vertex_count = 0, indice_count = 0;
	Vertex *vertex_list = nullptr;
	Uint16 *indice_list = nullptr;
	
	RG_SetMetadata (
		"https://www.retardedgames.com",
		"DicWholesome",
		"CC BY-NC",
		"Game",
		GAME_NAME,
		"0.1",
		"com.retardedgames.myroom"
	);

	context = RG_Init (GAME_NAME, 800, 800);
	
	if (!context)
	{
		SDL_Log ("FAILED TO CREATE GAME CONTEXT!");
		return SDL_APP_FAILURE;
	}
	
	RG_InitAudio (&context);
	
	
	actions = RG_LoadActionData ("open.json");
	action_list.push_back (RG_GetAction (actions, 0));
	action_list.push_back (RG_GetAction (actions, 1));
	RG_ActionDataFree (actions);
	
	for (Action& action : action_list)
	{
		SDL_Log ("Object name to be created: %s", action.create.name);
	}
	
	//SHADERS
	SDL_GPUShader *vShader = RG_LoadShader (
		context->device,
		"shader.vert",
		0,
		1,
		0,
		0
	);
	SDL_GPUShader *fShader = RG_LoadShader (
		context->device,
		"shader.frag",
		1,
		0,
		0,
		0
	);
	
	GLB *faggit_car = RG_GLBOpen  ("sedan-sports.glb");
	if (faggit_car)
	{
		Uint32 len = 0;
		float *uvs = RG_GLBGetMeshUVCoord (faggit_car, 0, &len);
		float *pos = RG_GLBGetMeshPositions (faggit_car, 0, &len);
		
		//len /= 3;
		vertex_list = (Vertex *)SDL_malloc (sizeof (Vertex) * len);
		vertex_count = len;
		
		vec2s lookup[] = {
			{0.0f, 1.0f-0.0f},
			{1.0f, 1.0f-0.0f},
			{0.0f, 1.0f-1.0f},
			{1.0f, 1.0f-1.0f},
		};
		
		for (Uint32 i = 0, uvi = 0; i < len; i++, uvi++)
		{
			if (uvi > 3) uvi = 0;
			
			vertex_list[i].pos.x = pos[i*3];
			vertex_list[i].pos.y = pos[i*3+1];
			vertex_list[i].pos.z = pos[i*3+2];
			
			//default color
			vertex_list[i].color = RGBA{255,255,255,255};
			
			//should fix this later
			SDL_memcpy (&(vertex_list[i].uv), (vec2s *)(&lookup[uvi]), sizeof (float)*2);
			
		}
		
		indice_list = RG_GLBGetMeshIndices (faggit_car, 0, &len);
		indice_count = len;
		SDL_Log ("INDICE COUNT: %d", indice_count);
	}
	
	
	SDL_Surface *surface = LoadImage ("walls/bober.bmp");
	if (!surface)
		SDL_Log ("ERROR: %s", SDL_GetError ());
	
	Uint32 verticeBuffSize = sizeof (Vertex) * vertex_count;
	Uint32 indexBuffSize = sizeof (Uint16) * indice_count;
	Uint32 pixelBuffSize = surface->w * surface->h * 4;
	
	RG_GenerateBuffers (
		&context,
		verticeBuffSize,
		indexBuffSize,
		pixelBuffSize,
		surface->w,
		surface->h
	);
	Uint8 *transferMem = (Uint8 *)SDL_MapGPUTransferBuffer (
		context->device,
		context->transferBuffer,
		false
	);
	SDL_memcpy (
		transferMem,
		vertex_list,
		verticeBuffSize
	);
	SDL_memcpy (
		transferMem + verticeBuffSize,
		indice_list,
		indexBuffSize
	);
	SDL_UnmapGPUTransferBuffer (context->device, context->transferBuffer);
	
	Uint8 *texTransferMem = (Uint8 *)SDL_MapGPUTransferBuffer (
		context->device,
		context->texTransferBuffer,
		false
	);
	SDL_memcpy (
		texTransferMem,
		surface->pixels,
		pixelBuffSize
	);
	SDL_UnmapGPUTransferBuffer (context->device, context->texTransferBuffer);
	
	//Begin CopyPass
	SDL_GPUCommandBuffer *cbuf = SDL_AcquireGPUCommandBuffer (context->device);
	SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass (cbuf);	
	RG_UploadToGPUBuffer (
		copyPass,
		context->transferBuffer,
		context->texTransferBuffer,
		context->vertexBuffer,
		context->indexBuffer,
		context->textureBuffer,
		surface->w,
		surface->h,
		verticeBuffSize,
		indexBuffSize
	);
	SDL_EndGPUCopyPass (copyPass);
	SDL_SubmitGPUCommandBuffer (cbuf);
	SDL_ReleaseGPUTransferBuffer (context->device, context->transferBuffer);
	SDL_ReleaseGPUTransferBuffer (context->device, context->texTransferBuffer);
	
	//Sampler
	SDL_GPUSamplerCreateInfo samplerCreateInfo = {
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE
		
	};
	context->sampler = SDL_CreateGPUSampler (context->device, &samplerCreateInfo);
	if (context->sampler == nullptr)
	{
		SDL_Log ("ERROR LOADING SAMPLER: %s", SDL_GetError ());
	}
	
	/*
	 * 
	 * 	CREATE GPU PIPELINE
	 * 
	 */

	SDL_GPUVertexAttribute vertexAttrs[] = {
		{
			.location = 0,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = 0
			
		},
		{
			.location = 1,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
			.offset = sizeof (vec3s)
		},
		{
			.location = 2,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = sizeof (vec3s) + sizeof (RGBA)
		}
	};
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.vertex_shader = vShader,
		.fragment_shader = fShader,
		.vertex_input_state = {
			.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){
				{
					.slot = 0,
					.pitch = sizeof (Vertex),
					.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
					.instance_step_rate = 0
					
				}
			},
			.num_vertex_buffers = 1,
			.vertex_attributes = vertexAttrs,
			.num_vertex_attributes = 3
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.depth_stencil_state = {
			.compare_op = SDL_GPU_COMPAREOP_LESS,
			.enable_depth_test = true,
			.enable_depth_write = true
		},
		.target_info = {
			.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
				.format = SDL_GetGPUSwapchainTextureFormat (
					context->device,
					context->window
				)
			}},
			.num_color_targets = 1,
			.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D24_UNORM,
			.has_depth_stencil_target = true
		}
	};
	context->pipeline = SDL_CreateGPUGraphicsPipeline (
		context->device,
		&pipelineCreateInfo
	);
	//Release shaders if they're not needed for other pipelines
	SDL_ReleaseGPUShader (context->device, vShader);
	SDL_ReleaseGPUShader (context->device, fShader);
	SDL_DestroySurface (surface);
	
	//RG_PlaySong (context, "nibba1.ogg");
	
	//pass context to next callback
	*appstate = context;

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent (void *appstate, SDL_Event *e)
{
	Context *context = (Context *)appstate;
	
    if (e->type == SDL_EVENT_QUIT)
    {
		return SDL_APP_SUCCESS;
	}
	else if (e->type == SDL_EVENT_KEY_DOWN)
	{
		SDL_Keycode key = e->key.key;
		
		if (key == SDLK_W)
		{
			context->z += 0.1f;
		}
		
		if (key == SDLK_S)
		{
			context->z -= 0.1f;
		}
		
		if (key == SDLK_A)
		{
			context->x += 0.1f;
		}
		
		if (key == SDLK_D)
		{
			context->x -= 0.1f;
		}
		
		if (key == SDLK_LEFT)
		{
			
		}
		
		if (key == SDLK_RIGHT)
		{
			
		}
	}

    return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate (void *appstate)
{
	Context *context = (Context *)appstate;
	SDL_GPUTexture *swapTexture = nullptr;
	float degrees, deltaTime;
	int winHeight, winWidth, newTicks;
	static int lastTicks = SDL_GetTicks ();
	static float rotation = 0.0f;
	static float rotSpeed = glm_rad (90.0f);
	mat4s SDL_ALIGNED(16) proj;
	mat4s SDL_ALIGNED(16) model;
	mat4s SDL_ALIGNED(16) trans;
	mat4s SDL_ALIGNED(16) iden;
	UBO SDL_ALIGNED(16) ubo;

	newTicks = SDL_GetTicks ();
	deltaTime = (newTicks - lastTicks) / 1000.0f;
	lastTicks = newTicks;
	rotation += rotSpeed * deltaTime;

	SDL_GetWindowSize (context->window, &winWidth, &winHeight);

	vec3s SDL_ALIGNED(8) trans_vec = vec3s{context->x, context->y-0.90, context->z};
	vec3s SDL_ALIGNED(8) rot_vec = vec3s{0.0f, 1.0f, 0.0f};
	proj = glms_perspective (glm_rad (90.0f), winWidth / winHeight, 0.0001f, 1000);
	trans = glms_translate_make (trans_vec);
	model = glms_rotate_make (rotation, rot_vec);
	iden = glms_mat4_identity();

	ubo.mvp = glms_mat4_mulN((mat4s *[]){ &proj, &iden, &model}, 3);

	SDL_GPUCommandBuffer *cbuf = SDL_AcquireGPUCommandBuffer (context->device);
	if (!cbuf)
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return SDL_APP_FAILURE;
	}

	if (!SDL_WaitAndAcquireGPUSwapchainTexture (
		cbuf,
		context->window,
		&swapTexture,
		nullptr,
		nullptr
	))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return SDL_APP_FAILURE;
	}
	
	if (swapTexture)
	{
		SDL_GPUColorTargetInfo colorTargetInfo = {0};
		colorTargetInfo.texture = swapTexture;
		colorTargetInfo.clear_color = SDL_FColor {0.0f, 0.0f, 0.3f, 1.0f};
		colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
		
		SDL_GPUDepthStencilTargetInfo depthTargetInfo = {0};
		depthTargetInfo.clear_depth = 1;
		depthTargetInfo.texture = context->depthBuffer;
		depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
		
		
		
		SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass (
			cbuf,
			&colorTargetInfo,
			1,
			&depthTargetInfo
		);
		SDL_BindGPUGraphicsPipeline (renderPass, context->pipeline);
		
		//Bind vertex and index buffers faggit
		RG_BindBuffers (
			renderPass,
			context->vertexBuffer,
			context->indexBuffer,
			context->textureBuffer,
			context->sampler
		);
		// UNIFORM DATA
		SDL_PushGPUVertexUniformData (cbuf,  0, &ubo, sizeof (UBO));
		
		//Draw the shit!
		//SDL_DrawGPUPrimitives (renderPass, 3, 1, 0, 0);
		SDL_DrawGPUIndexedPrimitives (renderPass, 2136, 1, 0, 0, 0);
		SDL_EndGPURenderPass (renderPass);
	}
	
	if (!SDL_SubmitGPUCommandBuffer (cbuf))
	{
		SDL_Log ("RG ERROR: %s", SDL_GetError ());
		return SDL_APP_FAILURE;
	}

    return SDL_APP_CONTINUE;
}

void
SDL_AppQuit (void *appstate, SDL_AppResult result)
{
	Context *context = (Context *)appstate;
	
	RG_Quit (context);

}
