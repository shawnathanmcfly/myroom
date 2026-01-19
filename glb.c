#include <SDL3/SDL.h>
#include <json-c/json.h>
#include "init.h"

#define ACCESSOR_COMPONENT_TYPE_S8				5120
#define ACCESSOR_COMPONENT_TYPE_U8				5121
#define ACCESSOR_COMPONENT_TYPE_S16				5122
#define ACCESSOR_COMPONENT_TYPE_U16				5123
#define ACCESSOR_COMPONENT_TYPE_U32				5125
#define ACCESSOR_COMPONENT_TYPE_F32				5126
#define ACCESSOR_TYPE_SCALAR					0
#define ACCESSOR_TYPE_VEC2						1
#define ACCESSOR_TYPE_VEC3						2
#define ACCESSOR_TYPE_VEC4						3
#define ACCESSOR_TYPE_MAT2						4
#define ACCESSOR_TYPE_MAT3						5
#define ACCESSOR_TYPE_MAT4						6

#define MESH_PRIMITIVE_TYPE_POSITION			0
#define MESH_PRIMITIVE_TYPE_NORMAL				1
#define MESH_PRIMITIVE_TYPE_TANGENT				2
#define MESH_PRIMITIVE_TYPE_TEXTCOORD_0			4

#define BUFFERVIEW_TARGET_ARRAY_BUFFER			34962
#define BUFFERVIEW_TARGET_ELEMENT_ARRAY_BUFFER	34963

typedef struct {
	Uint64 bin_len;
	Uint8 *bin;
} Buffer;

typedef union {
	Uint32 position, normal, tangent;
	Uint32 texcoord_0;
} Attribute;

typedef struct
{
	char *name;
	Attribute attr;
	Uint32 indices_index, material_index, attr_type;
	
} Mesh;

typedef struct
{
	Uint64 buffer_view_index;
	Uint64 element_count;
	Uint16 component_type;
	Uint8  type;
} Accessor;

typedef struct
{
	Uint64 buffer_index;
	Uint64 byte_offset;
	Uint64 byte_length;
	Uint8  target;
} BufferView;

typedef struct
{
	Uint64 accessor_count, bufferview_count, meshes_count, buffer_count;
	Accessor *accessors;
	BufferView *bufferviews;
	Mesh *meshes;
	Buffer *buffers;
	
} GLB;

static Mesh *
get_meshes (json_object *json)
{
	Mesh *meshes = NULL;
	json_object *cm = NULL, *e = NULL;
	Uint64 len = json_object_array_length (json);
	
	meshes = (Mesh *)SDL_malloc (sizeof (Mesh) * len);
	if (!meshes)
	{
		SDL_Log ("Not enough memory for GLB Meshes: %s", SDL_GetError ());
		return NULL;
	}
	
	for (Uint64 i = 0; i < len; i++)
	{
		json_object *prim = NULL, *attrs = NULL;
		struct json_object_iterator it, it_end;
		
		cm = json_object_array_get_idx (json, i);
		
		/* Get name of mesh */
		json_object_object_get_ex (cm, "name", &e);
		Uint32 name_len = json_object_get_string_len (e) + 1;
		meshes[i].name = (char *)SDL_calloc (name_len, sizeof (char));
		SDL_memcpy (meshes[i].name, json_object_get_string (e), name_len);
		
		/* Start parsing primitive data */
		json_object_object_get_ex (cm, "primitives", &e);
		/* Seems like meshes only have one primitve per mesh iter */
		prim = json_object_array_get_idx (e, 0);
		
		/* Get primitive indices and material indexes */
		json_object_object_get_ex (prim, "indices", &e);
		meshes[i].indices_index = json_object_get_uint64 (e);
		json_object_object_get_ex (prim, "material", &e);
		meshes[i].material_index = json_object_get_uint64 (e);
		
		/* Iterate over mesh primitives attributes */
		json_object_object_get_ex (prim, "attributes", &attrs);
		it = json_object_iter_begin (attrs);
		it_end = json_object_iter_end (attrs);
		while (!json_object_iter_equal (&it, &it_end))
		{
			const char *name = json_object_iter_peek_name (&it);
			e = json_object_iter_peek_value (&it);
			
			if (!SDL_strcasecmp ("POSITION", name))
			{
				meshes[i].attr.position = json_object_get_uint64 (e);
				meshes[i].attr_type = MESH_PRIMITIVE_TYPE_POSITION;
			}
			else if (!SDL_strcasecmp ("NORMAL", name))
			{
				meshes[i].attr.normal = json_object_get_uint64 (e);
				meshes[i].attr_type = MESH_PRIMITIVE_TYPE_NORMAL;
			}
			else if (!SDL_strcasecmp ("TANGENT", name))
			{
				meshes[i].attr.tangent = json_object_get_uint64 (e);
				meshes[i].attr_type = MESH_PRIMITIVE_TYPE_TANGENT;
			}
			else if (!SDL_strcasecmp ("TEXCOORD_0", name))
			{
				meshes[i].attr.texcoord_0 = json_object_get_uint64 (e);
				meshes[i].attr_type = MESH_PRIMITIVE_TYPE_TEXTCOORD_0;
			}
			else
			{
				SDL_Log ("Unknown Mesh primitive attrivute %s", name);
			}
			
			json_object_iter_next (&it);
		}
	}

	return meshes;
}

static Accessor *
get_accessors (json_object *json)
{
	Accessor *accessors = NULL;
	json_object *ca = NULL, *e = NULL;
	Uint64 len = json_object_array_length (json);
	
	accessors = (Accessor *)SDL_malloc (sizeof (Accessor) * len);
	if (!accessors)
	{
		SDL_Log ("Not enough memory for GLB Accessors: %s", SDL_GetError ());
		return NULL;
	}
	
	for (Uint64 i = 0; i < len; i++)
	{
		ca = json_object_array_get_idx (json, i);
		
		json_object_object_get_ex (ca, "bufferView", &e);
		accessors[i].buffer_view_index = json_object_get_uint64 (e);
		
		json_object_object_get_ex (ca, "componentType", &e);
		accessors[i].component_type = json_object_get_uint64 (e);
		
		json_object_object_get_ex (ca, "count", &e);
		accessors[i].element_count = json_object_get_uint64 (e);
		
		json_object_object_get_ex (ca, "type", &e);
		const char *type = json_object_get_string (e);
		if (!SDL_strcmp (type, "SCALAR"))
			accessors[i].type = ACCESSOR_TYPE_SCALAR;
		else if (!SDL_strcmp (type, "VEC2"))
			accessors[i].type = ACCESSOR_TYPE_VEC2;
		else if (!SDL_strcmp (type, "VEC3"))
			accessors[i].type = ACCESSOR_TYPE_VEC3;
		else if (!SDL_strcmp (type, "VEC4"))
			accessors[i].type = ACCESSOR_TYPE_VEC4;
		else if (!SDL_strcmp (type, "MAT2"))
			accessors[i].type = ACCESSOR_TYPE_MAT2;
		else if (!SDL_strcmp (type, "MAT3"))
			accessors[i].type = ACCESSOR_TYPE_MAT3;
		else if (!SDL_strcmp (type, "MAT4"))
			accessors[i].type = ACCESSOR_TYPE_MAT4;
	}
	
	return accessors;
}

static BufferView *
get_bufferviews (json_object *json)
{
	BufferView *bufferviews= NULL;
	json_object *cbv = NULL, *e = NULL;
	Uint64 len = json_object_array_length (json);
	
	bufferviews = (BufferView *)SDL_malloc (sizeof (BufferView) * len);
	if (!bufferviews)
	{
		SDL_Log ("Not enough memory for GLB BufferViews: %s", SDL_GetError ());
		return NULL;
	}
	
	for (Uint64 i = 0; i < len; i++)
	{
		cbv = json_object_array_get_idx (json, i);
		
		json_object_object_get_ex (cbv, "buffer", &e);
		bufferviews[i].buffer_index = json_object_get_uint64 (e);
		
		json_object_object_get_ex (cbv, "byteOffset", &e);
		bufferviews[i].byte_offset = json_object_get_uint64 (e);
		
		json_object_object_get_ex (cbv, "byteLength", &e);
		bufferviews[i].byte_length = json_object_get_uint64 (e);
		
		json_object_object_get_ex (cbv, "target", &e);
		bufferviews[i].target = json_object_get_uint64 (e);
	}
	
	return bufferviews;
}

static Buffer *
set_buffers (json_object *json, void *bin)
{
	Buffer *buffers= NULL;
	json_object *cb = NULL, *e = NULL;
	Uint64 len = json_object_array_length (json);
	
	buffers = (Buffer *)SDL_malloc (sizeof (Buffer) * len);
	if (!buffers)
	{
		SDL_Log ("Not enough memory for GLB Buffers: %s", SDL_GetError ());
		return NULL;
	}
	
	for (Uint64 i = 0; i < len; i++)
	{
		cb = json_object_array_get_idx (json, i);
		
		json_object_object_get_ex (cb, "byteLength", &e);
		buffers[i].bin_len = json_object_get_uint64 (e);
		buffers[i].bin = (Uint8 *)SDL_malloc (buffers[i].bin_len);
		SDL_memcpy (buffers[i].bin, bin, buffers[i].bin_len);
	}
	
	return buffers;
}

GLB *
RG_GLBOpen (const char *filename)
{
	bool fail = false;
	SDL_Storage *store = NULL;
	Uint64 dest_len = 0;
	Uint32 json_len = 0;
	char *json_data = NULL;
	void *bin_data = NULL;
	void *data = NULL;
	GLB *glb = (GLB *)SDL_malloc (sizeof (GLB));
	
	if (!glb) fail = true;
	
	store = SDL_OpenTitleStorage (RG_GetSubFolder ("models"), 0);
	if (!store)
	{
		SDL_Log ("Error opening game model directory: %s", SDL_GetError ());
		fail = true;
	}
	
	while (!SDL_StorageReady (store))
        SDL_Delay (1);
	
	if (!fail && !SDL_GetStorageFileSize (store, filename, &dest_len))
	{
		SDL_Log ("Failed to get size of %s: %s", filename, SDL_GetError ());
		fail = true;
	}
	
	if (!fail && dest_len == 0)
	{
		SDL_Log ("%s is empty.", filename);
		fail = true;
	}
	
	if (!fail) data = SDL_malloc (dest_len);
	if (!data)
	{
		SDL_Log ("Failed to allocate memory for file %s", filename);
		fail = true;
	}
	
	if (!fail && !SDL_ReadStorageFile (store, filename, data, dest_len))
	{
		SDL_Log ("Failed to parse data from %s: %s", filename, SDL_GetError ());
		fail = true;
	}
	
	if (!fail && *((Uint32 *)data) != 0x46546C67)
	{
		SDL_Log ("%s is not a valid GLB file.", filename);
		fail = true;
	}
	
	if (!fail && *((Uint32 *)data + 1) != 2)
	{
		SDL_Log ("%s needs to be at GTLF format version 2.", filename);
		fail = true;
	}
	
	/* Confirm first chunk is JSON */
	if (!fail && *((Uint32 *)data + 4) != 0x4E4F534A)
	{
		SDL_Log ("First chunk must be type JSON.");
		fail = true;
	}
	
	if (!fail)
	{
		json_len = *((Uint32 *)data + 3);
		json_data = SDL_malloc (json_len);
	}
	
	if (!fail && !json_data)
	{
		SDL_Log ("Failed to allocate memory for GLB JSON: %s", SDL_GetError ());
		fail = true;
	}
	
	if (fail)
	{
		SDL_free (glb); glb = NULL;
	}
	else
	{
		void *bin_data = data + sizeof (Uint32) * 5 + json_len;
		json_object *json = NULL, *object = NULL;
		
		bin_data += sizeof (Uint32) + sizeof (Uint32);
		
		SDL_memcpy (json_data, ((Uint32 *)data + 5), json_len);
		json = json_tokener_parse (json_data);
		
		json_object_object_get_ex (json, "accessors", &object);
		glb->accessor_count = json_object_array_length (object);
		glb->accessors = get_accessors (object);
		
		json_object_object_get_ex (json, "bufferViews", &object);
		glb->bufferview_count = json_object_array_length (object);
		glb->bufferviews = get_bufferviews (object);
		
		json_object_object_get_ex (json, "meshes", &object);
		glb->meshes_count = json_object_array_length (object);
		glb->meshes = get_meshes (object);
		
		/* Currently only supports 1 buffer */
		json_object_object_get_ex (json, "buffers", &object);
		glb->buffer_count = json_object_array_length (object);
		glb->buffers = set_buffers (object, bin_data);
		
		json_object_put (json); json = NULL;
		object = NULL;
	}
	
	SDL_free (data); data = NULL;
	SDL_free (json_data); json_data = NULL;
	SDL_CloseStorage (store); store = NULL;
	
	return glb;
}

void
RG_GLBClose (GLB *glb)
{
	if(!glb) return;
	
	/* Mesh name was dynamically allocated */
	for (int i = 0; i < glb->meshes_count; i++)
	{
		SDL_free (glb->meshes[i].name); glb->meshes[i].name = NULL;
	}
	
	SDL_free (glb->meshes); glb->meshes = NULL;
	SDL_free (glb->accessors); glb->accessors = NULL;
	SDL_free (glb->bufferviews); glb->bufferviews = NULL;
	
	for (int i = 0; i < glb->buffer_count; i++)
	{
		SDL_free (glb->buffers[i].bin); glb->buffers[i].bin = NULL;
	}
	
	SDL_free (glb->buffers); glb->buffers = NULL;
	SDL_free (glb); glb = NULL;
}
