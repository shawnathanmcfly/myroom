#ifndef _GLB_H
#define _GLB_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct GLB GLB;
typedef struct Mesh Mesh;

GLB *
RG_GLBOpen (const char *filename);

void
RG_GLBClose (GLB *glb);

Uint32
RG_GLBGetMeshCount (GLB *glb);

float *
RG_GLBGetMeshUVCoord (GLB *glb, Uint32 i, Uint32 *len);

float *
RG_GLBGetMeshPositions (GLB *glb, Uint32 i, Uint32 *len);

Uint16 *
RG_GLBGetMeshIndices (GLB *glb, Uint32 i, Uint32 *len);

#ifdef __cplusplus
}
#endif
#endif /* _GLB_H */
