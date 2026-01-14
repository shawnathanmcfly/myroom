#include "sprite.h"

Sprite *
RG_SpriteLoad (const char *filename)
{
	cgltf_options options;
	SDL_memset (&options, 0, sizeof (cgltf_options));
	cgltf_data *data = NULL;
	cgltf_result res = cgltf_parse_file (
		&options,
		RG_GetAssetFile ("models", filename),
		&data
	);
	
	if (res == cgltf_result_success && data->file_type == cgltf_file_type_glb)
	{
		
		//SDL_Log ("===== TIME TO GET 3D MODEL INFO FAGGIT! =====");
		//for (Uint64 i = 0; i < data->accessors_count; i++)
		//{
		//	
		//}
		
		SDL_Log ("DATA BUFFER VIEW COUNT: %d", data->buffer_views_count);
		SDL_Log ("ACCESSOR COUNT: %d", data->accessors_count);
		SDL_Log ("BUFFERS COUNT: %d", data->buffers_count);
		SDL_Log ("SCENES COUNT: %d", data->scenes_count);
		
		cgltf_free (data);
		data = nullptr;
	}
	
	return NULL;
}
