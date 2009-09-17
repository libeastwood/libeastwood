#include "StdDef.h"

#include "adl/emuopl.h"
#include "SDL/MixerPlayer.h"

namespace eastwood { namespace SDL {

MixerPlayer::MixerPlayer(int channels, int freq, uint16_t format, Copl* opl) :
    CadlPlayer(opl), _channels(channels), _freq(freq), _format(format)
{
}

MixerPlayer::MixerPlayer() :
   CadlPlayer(), _channels(0), _freq(0), _format(0)
{
  Mix_QuerySpec(&_freq, &_format, &_channels);

  _opl = new CEmuopl(_freq, (_format & AUDIO_U16LSB), (_channels > 1));

  init();
}

void MixerPlayer::callback(void *userdata, uint8_t *audiobuf, int len)
{
  MixerPlayer *self = (MixerPlayer *)userdata;
  static long	minicnt = 0;
  long		i, towrite = len / self->getsampsize();
  char		*pos = (char *)audiobuf;

  // Prepare audiobuf with emulator output
  while(towrite > 0) {
    while(minicnt < 0) {
      minicnt += self->_freq;
      self->playing = self->update();
    }
    i = std::min(towrite, (long)(minicnt / self->getrefresh() + 4) & ~3);
    self->_opl->update((short *)pos, i);
    pos += i * self->getsampsize(); towrite -= i;
    minicnt -= (long)(self->getrefresh() * i);
  }
}

}}
