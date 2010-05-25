/*
Copyright (C) 2000, 2001  Ryan Nunn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// XMIDI/MIDI Converter/Loader

#ifndef EASTWOOD_XMIFILE_H
#define EASTWOOD_XMIFILE_H

#include <bitset>
#include <deque>
#include <vector>

#include "eastwood/IStream.h"
#include "eastwood/OStream.h"
#include "eastwood/IffFile.h"

namespace eastwood {

struct midi_event;
enum MidiStatus {
    MIDI_STATUS_NOTE_OFF	= 0x80,
    MIDI_STATUS_NOTE_ON		= 0x90,
    MIDI_STATUS_AFTERTOUCH	= 0xA0,
    MIDI_STATUS_CONTROLLER	= 0xB0,
    MIDI_STATUS_PROG_CHANGE	= 0xC0,
    MIDI_STATUS_PRESSURE	= 0xD0,
    MIDI_STATUS_PITCH_WHEEL	= 0xE0,
    MIDI_STATUS_SYSEX		= 0xF0
};

class   XmiFile
{
    public:
	XmiFile(std::istream &source);
	~XmiFile();

	int32_t number_of_tracks()
	{
	    if (info.type != 1)
		return info.tracks;
	    else
		return 1;
	};

	// Retrieve it to a data source
	int32_t retrieve(uint32_t track, std::ostream &dest);	

	static void deleteEventList(midi_event *mlist);

    protected:
	struct  midi_descriptor
	{
	    uint16_t	type;
	    uint16_t	tracks;
	};

	midi_descriptor	info;


    private:
	XmiFile(); // No default constructor

	// List manipulation
	void createNewEvent(int32_t time);

	// Variable length quantity
	int32_t getVLQ(uint32_t &quant);
	int32_t getVLQ2(uint32_t &quant);
	int32_t putVLQ(OStream &dest, uint32_t value);	

	void movePatchVolAndPan(int32_t channel = -1);
	void duplicateAndMerge(int32_t num = 0);

	int32_t convertEvent(const int32_t time, const MidiStatus status, const int32_t size);
	int32_t convertSystemMessage(const int32_t time, const MidiStatus status);

	int32_t convertFiletoList(bool is_xmi);
	uint32_t convertListToMTrk(OStream &dest, midi_event *mlist);	

	int32_t extractTracksFromXmi();
	int32_t extractTracksFromMid();

	int32_t extractTracks();

	IffFile			_iff;
	midi_event		**events;
	std::vector<int16_t>	timing;

	midi_event		*list;
	midi_event		*current;

	std::bitset<16>		bank127;
	std::deque<bool>	fixed;

};

}

#endif //RANDGEN_XMIFILE_H
