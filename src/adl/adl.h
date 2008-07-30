/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2006 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * adl.h - ADL player adaption by Simon Peter <dn.tlp@gmx.net>
 */

#ifndef H_ADPLUG_ADLPLAYER
#define H_ADPLUG_ADLPLAYER

#include <inttypes.h>
#include <string>
#include <vector>

#include <SDL_mixer.h>
#include <SDL_rwops.h>

#include "opl.h"

class AdlibDriver;

class CadlPlayer
{
 public:
  CadlPlayer(SDL_RWops* rwop);
  ~CadlPlayer();

  std::vector<int> getSubsongs();
  Mix_Chunk* getSubsong(int Num);
  Mix_Chunk* getUpsampledSubsong(int Num, int TargetFrequency, Uint16 TargetFormat, int channels);

 private:
  Copl *opl;	// our OPL chip

  int numsubsongs, cursubsong;

  AdlibDriver *_driver;

  uint8_t _trackEntries[120];
  uint8_t *_soundDataPtr;
  int _sfxPlayingSound;

  uint8_t _sfxPriority;
  uint8_t _sfxFourthByteOfSong;

  int _numSoundTriggers;
  const int *_soundTriggers;

  bool init();

  bool load(SDL_RWops* rwop);
  bool update();
  void rewind(int subsong);

  void playTrack(uint8_t track);

  // refresh rate is fixed at 72Hz
  float getrefresh()
  {
      return 72.0f;
  }

  void process();
  void playSoundEffect(uint8_t track);
  void play(uint8_t track);
  void unk1();
  void unk2();
};

#endif
