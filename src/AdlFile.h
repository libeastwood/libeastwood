/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2006 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * adl.h - ADL player adaption by Simon Peter <dn.tlp@gmx.net>
 */

#ifndef EASTWOOD_ADLIBFILE_H
#define EASTWOOD_ADLIBFILE_H 

namespace eastwood {

class AdlibDriver;
class Copl;
class CadlPlayer
{
    public:
	CadlPlayer(Copl *newopl);
	CadlPlayer();
	virtual ~CadlPlayer();

	bool load(const std::istream &stream);
	bool update();
	void rewind(int subsong);
	virtual Copl* get_opl() { return _opl; }
	bool init();
	// refresh rate is fixed at 72Hz
	float getrefresh()
	{
	    return 72.0f;
	}

	unsigned int getsubsongs();
	std::string gettype() { return std::string("Westwood ADL"); }

    protected:
	AdlibDriver *_driver;
	Copl* _opl;
	bool playing;

    private:
	int numsubsongs, cursubsong;

	uint8_t _trackEntries[120];
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
