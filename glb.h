#ifndef _GLB_H
#define _GLB_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct GLB GLB;
typedef struct Mesh Mesh;
typedef struct Position Position;

GLB *
RG_GLBOpen (const char *filename);

void
RG_GLBClose (GLB *glb);

Uint32
RG_GLBGetMeshCount (GLB *glb);

Position *
RG_GLBGetMeshPositions (GLB *glb, Uint32 i);

#ifdef __cplusplus
}
#endif
#endif /* _GLB_H */
