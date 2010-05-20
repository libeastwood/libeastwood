/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2006 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * adl.h - ADL player adaption by Simon Peter <dn.tlp@gmx.net>
 */

#ifndef EASTWOOD_ADLIBFILE_H
#define EASTWOOD_ADLIBFILE_H 

#include "eastwood/IStream.h"

namespace eastwood {

class AdlibDriver;
class Copl;
class CadlPlayer
{
    public:
	CadlPlayer(Copl *newopl = NULL, bool v2 = false);
	virtual ~CadlPlayer();

	bool load(std::istream &stream);
	bool update();
	void rewind(int subsong);
	virtual Copl* get_opl() { return _opl; }
	// refresh rate is fixed at 72Hz
	float getrefresh()
	{
	    return 72.0f;
	}

	unsigned int getsubsongs();
	std::string gettype() { return std::string("Westwood ADL"); }

    protected:
	void init();

	AdlibDriver *_driver;
	Copl* _opl;

    private:
	int numsubsongs, cursubsong;

	bool _v2;
	uint8_t _trackEntries[500];
	uint8_t *_soundDataPtr;
	int _sfxPlayingSound;

	uint8_t _sfxPriority;
	uint8_t _sfxFourthByteOfSong;

	int _numSoundTriggers;
	const int *_soundTriggers;


	static const int _kyra1NumSoundTriggers;
	static const int _kyra1SoundTriggers[];



	void process();
	void playTrack(uint8_t track);
	bool isPlaying();

	void playSoundEffect(uint8_t track);
	void play(uint8_t track);
	void unk1();
	void unk2();

};

class AdlFile : public CadlPlayer
{
 public:
  AdlFile();
};

}
#endif // EASTWOOD_ADLIBFILE_H
