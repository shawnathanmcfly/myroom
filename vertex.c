#include "vertex.h"
#include "glb.h"

Vertex *
RG_VertexGetFromGLB (const char *filename)
{
	GLB *glb = RG_GLBOpen (filename);
	
	RG_GLBClose (glb); glb = NULL;
}
