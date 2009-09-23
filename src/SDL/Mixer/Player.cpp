#include "eastwood/StdDef.h"

#include "eastwood/adl/emuopl.h"
#include "eastwood/SDL/Mixer/Player.h"

namespace eastwood { namespace SDL { namespace Mixer {

Player::Player(int channels, int freq, uint16_t format, Copl* opl) :
    CadlPlayer(opl),
    _channels(channels), _freq(freq), _format(format), _playing(false)
{
}

Player::Player() :
   CadlPlayer(),
    _channels(0), _freq(0), _format(0), _playing(false)
{
  Mix_QuerySpec(&_freq, &_format, &_channels);

  _opl = new CEmuopl(_freq, (_format & AUDIO_U16LSB), (_channels > 1));

  init();
}

void Player::callback(void *userdata, uint8_t *audiobuf, int len)
{
  Player *self = (Player *)userdata;
  static long	minicnt = 0;
  long		i, towrite = len / self->getsampsize();
  char		*pos = (char *)audiobuf;

  // Prepare audiobuf with emulator output
  while(towrite > 0) {
    while(minicnt < 0) {
      minicnt += self->_freq;
      self->_playing = self->update();
    }
    i = std::min(towrite, (long)(minicnt / self->getrefresh() + 4) & ~3);
    self->_opl->update((short *)pos, i);
    pos += i * self->getsampsize(); towrite -= i;
    minicnt -= (long)(self->getrefresh() * i);
  }
}

}}}
