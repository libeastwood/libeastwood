#include "Animation.h"

Animation::Animation() {
	m_curFrameStartTime = SDL_GetTicks();
	m_frameDurationTime = 1;
	m_numFrames = 0;
	m_curFrame = 0;
	m_frame = NULL;
}

Animation::~Animation() {
	if(m_frame != NULL) {
		for(Uint32 i=0; i < m_numFrames; i++) {
			SDL_FreeSurface(m_frame[i]);
//			m_frame[i] = NULL;
		}
		free(m_frame);
	}
}

SDL_Surface *Animation::getFrame() {
	if(m_frame == NULL) {
		return NULL;
	}
	
	if((SDL_GetTicks() - m_curFrameStartTime) > m_frameDurationTime) {
		m_curFrameStartTime = SDL_GetTicks();
		m_curFrame++;
		if(m_curFrame >= m_numFrames) {
			m_curFrame = 0;
		}
	}
	return m_frame[m_curFrame];
}

void Animation::addFrame(SDL_Surface * newFrame, bool SetColorKey) {
	if((m_frame = (SDL_Surface **) realloc(m_frame,sizeof(SDL_Surface *) * (m_numFrames+1))) == NULL) {
		perror("Animation::addFrame()");
		exit(EXIT_FAILURE);
	}
	
		m_frame[m_numFrames] = newFrame;
	
	if(SetColorKey == true) {
		SDL_SetColorKey(m_frame[m_numFrames], SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
	}
	m_numFrames++;
}
