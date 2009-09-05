#include "SDL/MixerPlayer.h"

namespace eastwood { namespace SDL {

MixerPlayer::MixerPlayer(Copl* opl) : CadlPlayer(opl)
{
}

MixerPlayer::MixerPlayer() : CadlPlayer()
{
  Mix_QuerySpec(&_freq, &_format, &_channels);

  _opl = new CEmuopl(_freq, true, true);


  _driver = new AdlibDriver(opl);
  assert(_driver);

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
      minicnt += self->m_freq;
      self->playing = self->update();
    }
    i = std::min(towrite, (long)(minicnt / self->getrefresh() + 4) & ~3);
    self->opl->update((short *)pos, i);
    pos += i * self->getsampsize(); towrite -= i;
    minicnt -= (long)(self->getrefresh() * i);
  }
}

}}
