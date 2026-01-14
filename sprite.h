#ifndef _SPRITE_H
#define _SPRITE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include "init.h"
#include "cgltf.h"

typedef struct {
	char *name;
} Sprite;

Sprite *
RG_SpriteLoad (const char *filename);

#ifdef __cplusplus
}
#endif
#endif /* _SPRITE_H */
