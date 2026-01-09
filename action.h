#ifndef _ACTION_H
#define _ACTION_H
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL.h>
#include <cglm/struct.h>
#include <json-c/json.h>
#include "image.h"
#include "init.h"

#define RG_MAX_ACTION_NAME_LENGTH 40

enum {
	RG_ACTION_CREATE
};

typedef struct {
	char name[RG_MAX_ACTION_NAME_LENGTH];
	vec3s pos;
	float rot_x, rot_y;
	SDL_Surface *surface;
} A_Create;

typedef struct {
	Uint16 type;
	union {
		A_Create create;
	};
} Action;

typedef struct {
	struct json_object *data;
	Uint16 length;
} Actions;

Actions *
RG_LoadActionData (const char* filename);

Action
RG_GetAction (Actions *actions, Uint16 index);

int
RG_ActionDataFree (Actions *actions);

Uint16
RG_ActionDataGetLength (Actions *actions);

#ifdef __cplusplus
}
#endif
#endif /* _ACTION_H */
