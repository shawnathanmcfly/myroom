#include "vertex.h"
#include "glb.h"

Vertex *
RG_VertexGetFromGLB (const char *filename)
{
	Vertex *vertex_list = NULL;
	GLB *glb = RG_GLBOpen (filename);
	
	if (!glb)
	{
		SDL_Log ("Error loading GLB file %s", filename);
		return NULL;
	}
	
	vec3s *positions = RG_GLBGetMeshPositionsByName (glb, "body");
	
	RG_GLBClose (glb); glb = NULL;
	
	return vertex_list;
}
