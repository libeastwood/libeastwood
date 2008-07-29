#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <SDL.h>
#include "Gfx.h"

class Animation
{
public:
	Animation();
	~Animation();
	
	Image * getFrame();
	void setFrameRate(double FrameRate) {
		if(FrameRate == 0.0) {
			FrameDurationTime = 1;
		} else {
			FrameDurationTime = (int) (1000.0/FrameRate);
		}
	}
	
	void addFrame(Image * newFrame, bool SetColorKey = false);

private:
	Uint32 CurFrameStartTime;
	Uint32 FrameDurationTime;
	int curFrame;
	int NumFrames;
	Image ** Frame;
};

#endif // ANIMATION_H_INCLUDED

