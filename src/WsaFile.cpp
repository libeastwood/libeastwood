#include "Font.h"
#include "Log.h"

#include "WsaFile.h"

#include <SDL.h>
#include <SDL_endian.h>
#include <stdlib.h>
#include <string>

WsaFile::WsaFile(unsigned char *bufFileData, int bufSize, SDL_Palette *palette,
                SDL_Surface *lastframe, float setFps ) : Decode()
{
	Filedata = bufFileData;
	WsaFilesize = bufSize;
	m_palette = palette;
	
    LOG_INFO("WsaFile", "Loading wsa with size %d...", bufSize);
        
	if(WsaFilesize < 10) {
		LOG_ERROR("WsaFile", "No valid WSA-File: File too small!");
		exit(EXIT_FAILURE);
	}
	
	NumFrames = SDL_SwapLE16(*((Uint16*) Filedata) );
    LOG_INFO("WsaFile", "numframes = %d", NumFrames);

	SizeX = SDL_SwapLE16(*((Uint16*) (Filedata + 2)) );
	SizeY = SDL_SwapLE16(*((Uint16*) (Filedata + 4)) );
    LOG_INFO("WsaFile", "size %d x %d", SizeX, SizeY);
	
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

    LOG_INFO("WsaFile", "FramesPer1024ms = %d", FramesPer1024ms);
    LOG_INFO("WsaFile", "FPS = %.3f", fps);
	
	if(Index[0] == 0) {
		Index++;
		NumFrames--;
	}
	
	if(Filedata + WsaFilesize < (((unsigned char *) Index) + 4 * NumFrames)) {
		LOG_ERROR("WsaFile", "No valid WSA-File: File too small -2-!");
		exit(EXIT_FAILURE);		
	}
	
	if( (decodedFrames = (unsigned char*) calloc(1,SizeX*SizeY*NumFrames)) == NULL) {
		LOG_ERROR("WsaFile", "Unable to allocate memory for decoded WSA-Frames!");
		exit(EXIT_FAILURE);				
	}

    if (lastframe != NULL)
    {
        memcpy(decodedFrames, lastframe->pixels, SizeX*SizeY);
    }
	
	decodeFrames();
}

WsaFile::WsaFile() : Decode()
{
	WsaFilesize = -1;

    LOG_INFO("WsaFile", "Loading empty image as wsa...");
	
	NumFrames = 1;
	fps = 0.1;

    LOG_INFO("WsaFile", "FramesPer1024ms = %d", FramesPer1024ms);
    LOG_INFO("WsaFile", "FPS = %.3f", fps);
	decodedFrames = NULL;
}

WsaFile::~WsaFile()
{
	free(decodedFrames);
}

SDL_Surface *WsaFile::getSurface(Uint32 FrameNumber)
{
	if(FrameNumber >= NumFrames) {
		return NULL;
	}
	
	SDL_Surface *pic;
	unsigned char *Frame = decodedFrames + (FrameNumber * SizeX * SizeY);
	
	// create new picture surface
	if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,SizeX,SizeY,8,0,0,0,0))== NULL) 
	{
		return NULL;
	}
	
	SDL_SetColors(pic, m_palette->colors, 0, m_palette->ncolors);
	SDL_LockSurface(pic);

        //printf("%u\n", Image[0]);

	//Now we can copy line by line
	for(Uint16 y = 0; y < SizeY;y++) 
	{
		memcpy(	((unsigned char*) (pic->pixels)) + y * pic->pitch , Frame + y * SizeX, SizeX);
	}
		
	SDL_UnlockSurface(pic);

	return pic;	

}

void WsaFile::decodeFrames()
{
	unsigned char *dec80;
	
	for(Uint16 i=0;i<NumFrames;i++) 
	{
		if( (dec80 = (unsigned char*) calloc(1,SizeX*SizeY*2)) == NULL) 
		{
			LOG_ERROR("WsaFile", "Unable to allocate memory for decoded WSA-Frames!");
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
