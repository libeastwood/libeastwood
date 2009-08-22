#ifndef EASTWOOD_VOCFILE_H
#define EASTWOOD_VOCFILE_H

class Mix_Chunk;
class SDL_RWops;

/**
 Try to load a VOC from the RWop. Returns a pointer to Mix_Chunk.
 It is the callers responsibility to deallocate that data again later on
 with Mix_FreeChunk()!
	\param	rwop	The source SDL_RWops as a pointer. The sample is loaded from this VOC-File.
	\return	a pointer to the sample as a Mix_Chunk. NULL is returned on errors.
 */
extern Mix_Chunk* loadVOCFromStream(std::istream &stream);

#endif // EASTWOOD_VOCFILE_H

