#ifndef EASTWOOD_ANIMATION_H
#define EASTWOOD_ANIMATION_H

#include <SDL.h>

class Animation
{
public:
	Animation();
	~Animation();
	
	SDL_Surface *getFrame();
	void setFrameRate(float frameRate) {
		if(frameRate == 0.0) {
			m_frameDurationTime = 1;
		} else {
			m_frameDurationTime = (int) (1000.0/frameRate);
		}
	}
	
	void addFrame(SDL_Surface *newFrame, bool SetColorKey = false);
	Uint32 getNumFrames() { return m_numFrames; }
	Uint32 getFrameDurationTime() { return m_frameDurationTime; }
	Uint32 getCurFrame() { return m_curFrame; }

private:
	Uint32 m_curFrameStartTime;
	Uint32 m_frameDurationTime;
	Uint32 m_curFrame;
	Uint32 m_numFrames;
	SDL_Surface **m_frame;
};

#endif // EASTWOOD_ANIMATION_H
