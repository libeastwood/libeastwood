#ifndef VOCFILE_H_INCLUDED
#define VOCFILE_H_INCLUDED

#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_mixer.h>

/**
 Try to load a VOC from the RWop. Returns a pointer to Mix_Chunk.
 It is the callers responsibility to deallocate that data again later on
 with Mix_FreeChunk()!
	\param	rwop	The source SDL_RWops as a pointer. The sample is loaded from this VOC-File.
	\param	freesrc	A non-zero value mean it will automatically close/free the src for you.
	\return	a pointer to the sample as a Mix_Chunk. NULL is returned on errors.
 */
extern Mix_Chunk* LoadVOC_RW(SDL_RWops* rwop, int freesrc);

#endif // VOCFILE_H_INCLUDED

