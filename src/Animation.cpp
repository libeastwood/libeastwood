#include "pakfile/Animation.h"
#include <stdio.h>
#include <stdlib.h>
Animation::Animation() {
	CurFrameStartTime = SDL_GetTicks();
	FrameDurationTime = 1;
	NumFrames = 0;
	curFrame = 0;
	Frame = NULL;
}

Animation::~Animation() {
	if(Frame != NULL) {
		for(int i=0; i < NumFrames; i++) {
			SDL_FreeSurface(Frame[i]->getSurface());
//			Frame[i] = NULL;
		}
		free(Frame);
	}
}

Image *  Animation::getFrame() {
	if(Frame == NULL) {
		return NULL;
	}
	
	if((SDL_GetTicks() - CurFrameStartTime) > FrameDurationTime) {
		CurFrameStartTime = SDL_GetTicks();
		curFrame++;
		if(curFrame >= NumFrames) {
			curFrame = 0;
		}
	}
	return Frame[curFrame];
}

void Animation::addFrame(Image * newFrame, bool SetColorKey) {
	if((Frame = (Image **) realloc(Frame,sizeof(Image *) * (NumFrames+1))) == NULL) {
		perror("Animation::addFrame()");
		exit(EXIT_FAILURE);
	}
	
		Frame[NumFrames] = newFrame;
	
	if(SetColorKey == true) {
		SDL_SetColorKey((Frame[NumFrames])->getSurface(), SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}
	NumFrames++;
}
