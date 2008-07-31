/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2006 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * adl.h - ADL player adaption by Simon Peter <dn.tlp@gmx.net>
 */

#ifndef EASTWOOD_ADLIBFILE_H
#define EASTWOOD_ADLIBFILE_H 

#include <string>
#include <inttypes.h>


#include <SDL.h>
#include <SDL_mixer.h>

class AdlibDriver;
class Copl;
class CadlPlayer
{
 public:
  CadlPlayer(Copl *newopl);
  CadlPlayer();
  ~CadlPlayer();

  static void callback(void *, Uint8 *, int);
  bool load(uint8_t *bufFiledata, int bufsize);
  bool update();
  void rewind(int subsong);
  Copl* get_opl() { return opl; }
  bool init();
  // refresh rate is fixed at 72Hz
  float getrefresh()
    {
      return 72.0f;
    }

  unsigned int getsubsongs();
  std::string gettype() { return std::string("Westwood ADL"); }

 private:
  int numsubsongs, cursubsong;

  AdlibDriver *_driver;
  Copl* opl;
  
  uint8_t _trackEntries[120];
  uint8_t *_soundDataPtr;
  int _sfxPlayingSound;

  uint8_t _sfxPriority;
  uint8_t _sfxFourthByteOfSong;

  int _numSoundTriggers;
  const int *_soundTriggers;
  
  int m_channels;
  int m_freq;
  Uint16 m_format;
  
  bool playing; //FIXME: It could be used for something, I hope.

  static const int _kyra1NumSoundTriggers;
  static const int _kyra1SoundTriggers[];


  
  unsigned char getsampsize() { 
        return m_channels * (m_format == AUDIO_U8 ? 1 : 2); }
        
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

#endif // EASTWOOD_ADLIBFILE_H
