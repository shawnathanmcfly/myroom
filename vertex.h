#ifndef _VERTEX_H
#define _VERTEX_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>

typedef struct
{
	Uint8 r, g, b, a;
} RGBA;

typedef struct
{
	vec3s SDL_ALIGNED (8) pos;
	RGBA color;
	vec2s uv;
} Vertex;

Vertex *
RG_VertexGetFromGLB (const char *filename);

#ifdef __cplusplus
}
#endif
#endif /* _VERTEX_H */
