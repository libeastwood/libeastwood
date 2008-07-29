#include <SDL_endian.h>
#include <stdlib.h>
#include <string>


#include "Font.h"
#include "Gfx.h"
#include "Log.h"

#include "pakfile/Wsafile.h"
#include "pakfile/Cpsfile.h"


Wsafile::Wsafile(uint8_t * bufFiledata, int bufsize, 
                SDL_Surface* lastframe, float setFps ) : Decode()
{
	Filedata = bufFiledata;
	WsaFilesize = bufsize;
	
    LOG_INFO("Wsafile", "Loading wsa with size %d...", bufsize);
        
	if(WsaFilesize < 10) {
		LOG_ERROR("Wsafile", "No valid WSA-File: File too small!");
		exit(EXIT_FAILURE);
	}
	
	NumFrames = SDL_SwapLE16(*((Uint16*) Filedata) );
    LOG_INFO("Wsafile", "numframes = %d", NumFrames);

	SizeX = SDL_SwapLE16(*((Uint16*) (Filedata + 2)) );
	SizeY = SDL_SwapLE16(*((Uint16*) (Filedata + 4)) );
    LOG_INFO("Wsafile", "size %d x %d", SizeX, SizeY);
	
	if( ((unsigned short *) Filedata)[4] == 0) {
		Index = (Uint32 *) (Filedata + 10);
		FramesPer1024ms = SDL_SwapLE32( *((Uint32*) (Filedata+6)) );
	} 
	else 
	{
		Index = (Uint32 *) (Filedata + 8);
		FramesPer1024ms = SDL_SwapLE16( *((Uint16*) (Filedata+6)) );
	}

    // surely /1000.0f not 100?!
	if(setFps)
		fps = setFps;
	else
		fps = (FramesPer1024ms / 1024.0f) / 100.0f;

    LOG_INFO("Wsafile", "FramesPer1024ms = %d", FramesPer1024ms);
    LOG_INFO("Wsafile", "FPS = %.3f", fps);
	
	if(Index[0] == 0) {
		Index++;
		NumFrames--;
	}
	
	if(Filedata + WsaFilesize < (((unsigned char *) Index) + 4 * NumFrames)) {
		LOG_ERROR("Wsafile", "No valid WSA-File: File too small -2-!");
		exit(EXIT_FAILURE);		
	}
	
	if( (decodedFrames = (unsigned char*) calloc(1,SizeX*SizeY*NumFrames)) == NULL) {
		LOG_ERROR("Wsafile", "Unable to allocate memory for decoded WSA-Frames!");
		exit(EXIT_FAILURE);				
	}

    if (lastframe != NULL)
    {
        memcpy(decodedFrames, lastframe->pixels, SizeX*SizeY);
    }
	
	decodeFrames();
}

Wsafile::Wsafile() : Decode()
{
	WsaFilesize = -1;

    LOG_INFO("Wsafile", "Loading empty image as wsa...");
	
	NumFrames = 1;
	fps = 0.1;

    LOG_INFO("Wsafile", "FramesPer1024ms = %d", FramesPer1024ms);
    LOG_INFO("Wsafile", "FPS = %.3f", fps);
	decodedFrames = NULL;
}

Wsafile::~Wsafile()
{
	free(decodedFrames);
}

Image * Wsafile::getPicture(Uint32 FrameNumber, SDL_Palette *palette)
{
	Image* img;
	if(WsaFilesize == -1){
		img = new Image(UPoint(1,1));
		return img;
	}

	if(FrameNumber >= NumFrames) {
		return NULL;
	}
	
	SDL_Surface * pic;
	uint8_t * Frame = decodedFrames + (FrameNumber * SizeX * SizeY);
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,SizeX,SizeY,8,0,0,0,0))== NULL) 
	{
		return NULL;
	}
	
	//printf("Frame Nr.: %d (Size: %dx%d)\n",FrameNumber,SizeX,SizeY);
			
    /*
        printf("%u %u %u\n", palette->colors[12].r,
                             palette->colors[12].g,
                             palette->colors[12].b);
    */
        
	SDL_SetColors(pic, palette->colors, 0, palette->ncolors);
	SDL_LockSurface(pic);

        //printf("%u\n", Image[0]);

	//Now we can copy line by line
	for(int y = 0; y < SizeY;y++) 
	{
		memcpy(	((uint8_t*) (pic->pixels)) + y * pic->pitch , Frame + y * SizeX, SizeX);
	}
		
	SDL_UnlockSurface(pic);
	
	img = new Image(pic);

	return img;

}

/// Returns an animation
/**
	This method returns a new animation object with all pictures from startindex to endindex
	in it. The returned pointer should be freed with delete if no longer needed. If an error
	occured, NULL is returned.
	\param	startindex	index of the first picture
	\param	endindex	index of the last picture
	\param	DoublePic	if true, the picture is scaled up by a factor of 2
	\param	SetColorKey	if true, black is set as transparency
	\return	a new animation object or NULL on error
*/
Animation* Wsafile::getAnimation(unsigned int startindex, unsigned int endindex, SDL_Palette *palette, bool SetColorKey)
{
	Animation* tmpAnimation;
	Image* tmp;
	
	if((tmpAnimation = new Animation()) == NULL) {
		return NULL;
	}
	
	for(unsigned int i = startindex; i <= endindex; i++) {
		if((tmp = getPicture(i, palette)) == NULL) {
			delete tmpAnimation;
			return NULL;
		}
		tmpAnimation->addFrame(tmp,SetColorKey);
	}
	return tmpAnimation;
}

void Wsafile::decodeFrames()
{
	unsigned char *dec80;
	
	for(int i=0;i<NumFrames;i++) 
	{
		if( (dec80 = (unsigned char*) calloc(1,SizeX*SizeY*2)) == NULL) 
		{
			LOG_ERROR("Wsafile", "Unable to allocate memory for decoded WSA-Frames!");
			exit(EXIT_FAILURE);	
		}

		decode80(Filedata + SDL_SwapLE32(Index[i]), dec80, 0);
	
		decode40(dec80, decodedFrames + i * SizeX * SizeY);

		free(dec80);
		
		if (i < NumFrames - 1) {
			memcpy(decodedFrames + (i+1) * SizeX * SizeY, decodedFrames + i * SizeX * SizeY,SizeX * SizeY);
		}
	}
}
