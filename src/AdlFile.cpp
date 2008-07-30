#include <SDL_mixer.h>

#include "adl/emuopl.h"
#include "adl/adl.h"
#include "adl/fmopl.h"
#include "AdlFile.h"

AdlFile::AdlFile()
{
	Mix_QuerySpec(&m_freq, &m_format, &m_channels);	
	m_opl = new CEmuopl(m_freq, true, true);
	m_adlPlayer = new CadlPlayer(m_opl);
}

void AdlFile::callback(void *userdata, Uint8 *audiobuf, int len)
{
	AdlFile *self = (AdlFile *)userdata;
	static long   minicnt = 0;
	long          i, towrite = len / self->getsampsize();
	char          *pos = (char *)audiobuf;

	// Prepare audiobuf with emulator output
	while(towrite > 0) {
		while(minicnt < 0) {
			minicnt += self->m_freq;
			self->playing = self->m_adlPlayer->update();
		}
		i = std::min(towrite, (long)(minicnt / self->m_adlPlayer->getrefresh() + 4) & ~3);
		self->m_opl->update((short *)pos, i);
		pos += i * self->getsampsize(); towrite -= i;
		minicnt -= (long)(self->m_adlPlayer->getrefresh() * i);
	}
}

