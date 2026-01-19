#ifndef _GLB_H
#define _GLB_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct GLB GLB;

GLB *
RG_GLBOpen (const char *filename);

void
RG_GLBClose (GLB *glb);

#ifdef __cplusplus
}
#endif
#endif /* _GLB_H */
