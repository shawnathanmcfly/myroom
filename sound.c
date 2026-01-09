#include "sound.h"

bool
RG_InitAudio (Context **context)
{
	if (!MIX_Init ())
		return false;
	
	(*context)->mixer = MIX_CreateMixerDevice (
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		NULL
	);

	return true;
}

bool
RG_PlaySong (Context *context, const char* filename)
{
	MIX_Audio *song = NULL;
	static MIX_Track *track = NULL;
	bool success = true;
		
	song = MIX_LoadAudio (
		context->mixer,
		RG_GetAssetFile ("music", filename),
		false
	);
	
	if (!song)
	{
		SDL_Log ("Error loading song '%s': %s", filename, SDL_GetError ());
		return false;
	}

	if (track)
	{
		MIX_StopTrack (track, 0);
		MIX_DestroyTrack (track);
		track = NULL;
	}
	
	track = MIX_CreateTrack (context->mixer);
		
	if (!track)
	{
		success = false;
		SDL_Log ("Failed to create track: %s", SDL_GetError ());
	}
	else
	{
		MIX_SetTrackAudio (track, song);
	}
	
	MIX_DestroyAudio (song);
	
	SDL_PropertiesID props = SDL_CreateProperties ();
	SDL_SetNumberProperty (props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	MIX_PlayTrack (track, props);
	SDL_DestroyProperties (props);
	
	return success;
}
