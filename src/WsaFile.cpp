#include <SDL.h>

#include "StdDef.h"

#include "Exception.h"
#include "Font.h"
#include "Log.h"
#include "WsaFile.h"

using namespace eastwood;

WsaFile::WsaFile(unsigned char *bufFileData, int bufSize, SDL_Palette *palette,
                SDL_Surface *lastframe, float setFps ) : Decode(),
    _fileData(bufFileData), _wsaFileSize(bufSize), _palette(palette)
{
    LOG_INFO("WsaFile", "Loading wsa with size %d...", bufSize);

    if(_wsaFileSize < 10)
	throw(Exception(LOG_ERROR, "WsaFile", "No valid WSA-File: File too small!"));

    _numFrames = htole16(*((uint16_t*) _fileData) );
    LOG_INFO("WsaFile", "numframes = %d", _numFrames);

    _sizeX = htole16(*((uint16_t*) (_fileData + 2)) );
    _sizeY = htole16(*((uint16_t*) (_fileData + 4)) );
    LOG_INFO("WsaFile", "size %d x %d", _sizeX, _sizeY);

    if( ((unsigned short *) _fileData)[4] == 0) {
	_index = (uint32_t *) (_fileData + 10);
	_framesPer1024ms = htole32( *((uint32_t*) (_fileData+6)) );
    } 
    else 
    {
	_index = (uint32_t *) (_fileData + 8);
	_framesPer1024ms = htole16( *((uint16_t*) (_fileData+6)) );
    }

    // surely /1000.0f not 100?!
    if(setFps)
	_fps = setFps;
    else
	_fps = (_framesPer1024ms / 1024.0f) / 100.0f;

    LOG_INFO("WsaFile", "_framesPer1024ms = %d", _framesPer1024ms);
    LOG_INFO("WsaFile", "FPS = %.3f", _fps);

    if(_index[0] == 0) {
	_index++;
	_numFrames--;
    }

    if(_fileData + _wsaFileSize < (((unsigned char *) _index) + 4 * _numFrames))
	throw(Exception(LOG_ERROR, "WsaFile", "No valid WSA-File: File too small -2-!"));

    if( (_decodedFrames = (unsigned char*) calloc(1,_sizeX*_sizeY*_numFrames)) == NULL)
	throw(std::bad_alloc());

    if (lastframe != NULL)
    {
	memcpy(_decodedFrames, lastframe->pixels, _sizeX*_sizeY);
    }

    decodeFrames();
}

WsaFile::WsaFile() : Decode(), _wsaFileSize(-1), _numFrames(1), _fps(0.1), _decodedFrames(NULL)
{

    LOG_INFO("WsaFile", "Loading empty image as wsa...");

    LOG_INFO("WsaFile", "_framesPer1024ms = %d", _framesPer1024ms);
    LOG_INFO("WsaFile", "FPS = %.3f", _fps);
}

WsaFile::~WsaFile()
{
    free(_decodedFrames);
}

SDL_Surface *WsaFile::getSurface(uint32_t frameNumber)
{
    if(frameNumber >= _numFrames) {
	return NULL;
    }

    SDL_Surface *pic;
    unsigned char *frame = _decodedFrames + (frameNumber * _sizeX * _sizeY);

    // create new picture surface
    if((pic = SDL_CreateRGBSurface(SDL_SWSURFACE,_sizeX,_sizeY,8,0,0,0,0))== NULL) 
    {
	return NULL;
    }

    SDL_SetColors(pic, _palette->colors, 0, _palette->ncolors);
    SDL_LockSurface(pic);

    //printf("%u\n", Image[0]);

    //Now we can copy line by line
    for(uint16_t y = 0; y < _sizeY;y++) 
    {
	memcpy(	((unsigned char*) (pic->pixels)) + y * pic->pitch , frame + y * _sizeX, _sizeX);
    }

    SDL_UnlockSurface(pic);

    return pic;	

}

void WsaFile::decodeFrames()
{
	unsigned char *dec80;
	
	for(uint16_t i=0;i<_numFrames;i++) 
	{
		if( (dec80 = (unsigned char*) calloc(1,_sizeX*_sizeY*2)) == NULL) 
	    	    throw(std::bad_alloc());

		decode80(_fileData + htole32(_index[i]), dec80, 0);
	
		decode40(dec80, _decodedFrames + i * _sizeX * _sizeY);

		free(dec80);
		
		if (i < _numFrames - 1) {
			memcpy(_decodedFrames + (i+1) * _sizeX * _sizeY, _decodedFrames + i * _sizeX * _sizeY,_sizeX * _sizeY);
		}
	}
}
