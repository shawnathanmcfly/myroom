#include "action.h"

Actions *
RG_LoadActionData (const char* filename)
{
	struct json_object *base_json = NULL, *json_array = NULL;
	Actions *actions;
	
	actions = (Actions *)SDL_malloc (sizeof (Actions));
	if (!actions)
	{
		SDL_Log ("Failed to allocate memory for ActionData file %s", filename);
		return NULL;
	}
	
	base_json = json_object_from_file (RG_GetAssetFile ("actions", filename));
	if (base_json == NULL)
	{	
		SDL_Log ("Error loading Json: %s", json_util_get_last_err ());
		SDL_free (actions);
		actions = NULL;
	}
	
	json_object_object_get_ex (base_json, "actions", &json_array);
	actions->data = json_object_get (json_array);
	actions->length = json_object_array_length (actions->data);
	
	json_object_put (base_json);
	
	base_json = NULL;
	json_array = NULL;
	
	return actions;
}

Action
RG_GetAction (Actions *actions, Uint16 index)
{
	 struct json_object *cur_action = NULL;
	 struct json_object *element = NULL;
	 const char *action_type = NULL;
	 const char *path = NULL;
	 Action action;
	 
	 cur_action = json_object_array_get_idx (actions->data, index);
	 json_object_object_get_ex (cur_action, "type", &element);
	 
	 action_type = json_object_get_string (element);
	 
	 if (!SDL_strcasecmp (action_type, "CREATE"))
	 {
		 action.type = RG_ACTION_CREATE;
		 
		 /* Object name*/
		 json_object_object_get_ex (cur_action, "name", &element);
		 SDL_strlcpy (
			action.create.name,
			json_object_get_string (element),
			RG_MAX_ACTION_NAME_LENGTH
		);
		
		/* Object X Y Z coord */
		json_object_object_get_ex (cur_action, "x", &element);
		action.create.pos.x = json_object_get_double (element);
		
		json_object_object_get_ex (cur_action, "y", &element);
		action.create.pos.y = json_object_get_double (element);
		
		json_object_object_get_ex (cur_action, "z", &element);
		action.create.pos.z = json_object_get_double (element);
		
		json_object_object_get_ex (cur_action, "image", &element);
		path = json_object_get_string (element);
		action.create.surface = LoadImage (path);
		
		action.create.rot_x = 0;
		action.create.rot_y = 0;
		
	 }
	 
	 return action;
}

int
RG_ActionDataFree (Actions *actions)
{
	int success = json_object_put (actions->data);
	actions->data = NULL;
	
	SDL_free (actions);
	actions = NULL;
	
	return success;
}


Uint16
RG_ActionDataGetLength (Actions *actions)
{
	if (!actions)
	{
		SDL_Log ("Passed Actions object is null!");
		return 0;
	}
	
	return actions->length;
}
