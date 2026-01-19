#ifndef _GLB_H
#define _GLB_H
#ifdef __cplusplus
extern "C" {
#endif

#include <cglm/cglm.h>
#include <cglm/struct.h>

typedef struct GLB GLB;

GLB *
RG_GLBOpen (const char *filename);

void
RG_GLBClose (GLB *glb);

vec3s *
RG_GLBGetMeshPositionsByName (GLB *glb, const char *name);

#ifdef __cplusplus
}
#endif
#endif /* _GLB_H */
