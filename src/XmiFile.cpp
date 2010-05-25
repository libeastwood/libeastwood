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


#include "eastwood/XmiFile.h"

// XMIDI Controllers
#define XMIDI_CONTROLLER_FOR_LOOP	116
#define XMIDI_CONTROLLER_NEXT_BREAK	117

// Maximum number of for loops we'll allow
#define XMIDI_MAX_FOR_LOOP_COUNT	128

// This is used to correct incorrect patch, vol and pan changes in midi files
// The bias is just a value to used to work out if a vol and pan belong with a 
// patch change. This is to ensure that the default state of a midi file is with
// the tracks centred, unless the first patch change says otherwise.
#define PATCH_VOL_PAN_BIAS	5

namespace eastwood {

struct midi_event
{
    int32_t			time;
    EventType			status;

    uint8_t			data[2];

    std::vector<uint8_t>	buffer;

    midi_event			*next;

    midi_event() : time(0), status(), buffer(), next(NULL)
    {
    }

};

// Constructor
XmiFile::XmiFile(std::istream &source) :
    info(), _iff(source), _events(), timing(NULL), list(NULL), current(NULL), bank127(), fixed()
{
    extractTracks();
}

XmiFile::~XmiFile()
{
    /*
    if (events) {
	for (int32_t i=0; i < info.tracks; i++)
	    deleteEventList (events[i]);
	delete [] events;
    }*/
}

int32_t XmiFile::retrieve (uint32_t track, std::ostream &Dest)
{
    OStream &dest = reinterpret_cast<OStream&>(Dest);
    int32_t len = 0;

    if (_events.empty())
	throw(Exception(LOG_ERROR, "XmiFile::retrieve()", "No midi data in loaded."));

    // Convert type 1 midi's to type 0
    if (info.type == 1) {
	duplicateAndMerge(-1);

	/*for (int32_t i=0; i < info.tracks; i++)
	    deleteEventList(events[i]);
*/
	//delete [] events;
	_events.resize(1);// = new midi_event *[1];
	_events[0] = list;

	info.tracks = 1;
	info.type = 0;
    }

    if (track >= info.tracks)
	throw(Exception(LOG_ERROR, "XmiFile::retrieve()", "Can't retrieve MIDI data, track out of range"));

    // And fix the midis if they are broken
    if (!fixed[track]) {
	list = _events[track];
	movePatchVolAndPan();
	fixed[track] = true;
	_events[track] = list;
    }

    dest.put('M');
    dest.put('T');
    dest.put('h');
    dest.put('d');

    dest.putU32BE(6);

    dest.putU16BE(0);
    dest.putU16BE(1);
    dest.putU16BE(static_cast<uint16_t>(timing[track]));

    len = convertListToMTrk(dest, _events[track]);

    return len + 14;
}

// Sets current to the new event and updates list
void XmiFile::createNewEvent(int32_t time)
{
    if (!list) {
	list = current = new midi_event;
	if (time)
	    current->time = time;
	return;
    }

    if (time < 0) {
	midi_event *event = new midi_event;
	event->next = list;
	list = current = event;
	return;
    }

    if (current->time > time)
	current = list;

    while (current->next) {
	if (current->next->time > time)	{
	    midi_event *event = new midi_event;

	    event->next = current->next;
	    current->next = event;
	    current = event;
	    current->time = time;
	    return;
	}

	current = current->next;
    }

    current->next = new midi_event;
    current = current->next;
    current->time = time;
}


// Conventional Variable Length Quantity
int32_t XmiFile::getVLQ(uint32_t &quant)
{
    int32_t i;
    quant = 0;
    uint32_t data;
    IffChunk chunk = _iff.getChunk();

    for (i = 0; i < 4; i++) {
	data = chunk->get();
	quant <<= 7;
	quant |= data & 0x7F;

	if (!(data & EV_NOTE_OFF)) {
	    i++;
	    break;
	}

    }
    return i;
}

// XMIDI Delta Variable Length Quantity
int32_t XmiFile::getVLQ2(uint32_t &quant)
{
    int32_t i;
    quant = 0;
    int32_t data = 0;
    IffChunk chunk = _iff.getChunk();

    for (i = 0; i < 4; i++) {
	data = chunk->get();
	if (data & EV_NOTE_OFF) {
	    chunk->seekg(-1, std::ios::cur);
	    break;
	}
	quant += data;
    }
    return i;
}

int32_t XmiFile::putVLQ(OStream &dest, uint32_t value)
{
    int32_t buffer;
    int32_t i = 1;
    buffer = value & 0x7F;
    while (value >>= 7) {
	buffer <<= 8;
	buffer |= ((value & 0x7F) | 0x80);
	i++;
    }
    for (int32_t j = 0; j < i; j++) {
	dest.put(buffer & 0xFF);
	buffer >>= 8;
    }

    return i;
}

// MovePatchVolAndPan
//
// This little function attempts to correct errors in midi files
// that relate to patch, volume and pan changing
void XmiFile::movePatchVolAndPan(int32_t channel)
{
    if (channel == -1) {
	for (int32_t i = 0; i < 16; i++)
	    movePatchVolAndPan(i);

	return;
    }

    midi_event *patch = NULL;
    midi_event *vol = NULL;
    midi_event *pan = NULL;
    midi_event *bank = NULL;
    midi_event *temp;

    for (current = list; current; ) {
	if (!patch && (current->status & 0xf0) == EV_PROG && (current->status & 0xf) == channel)
	    patch = current;
	else if (!vol && (current->status & 0xf0) == EV_CONTROL && current->data[0] == 7 && (current->status & 0xf) == channel)
	    vol = current;
	else if (!pan && (current->status & 0xf0) == EV_CONTROL && current->data[0] == 10 && (current->status & 0xf) == channel)
	    pan = current;
	else if (!bank && (current->status & 0xf0) == EV_CONTROL && current->data[0] == 0 && (current->status & 0xf) == channel)
	    bank = current;

	if (pan && vol && patch) break;

	if (current) current = current->next;
	else current = list;
    }

    // Got no patch change, return and don't try fixing it
    if (!patch) return;


    // Copy Patch Change Event
    temp = patch;
    patch = new midi_event;
    patch->time = temp->time;
    patch->status = static_cast<EventType>(channel | EV_PROG);
    patch->data[0] = temp->data[0];


    // Copy Volume
    if (vol && (vol->time > patch->time+PATCH_VOL_PAN_BIAS || vol->time < patch->time-PATCH_VOL_PAN_BIAS))
	vol = NULL;

    temp = vol;
    vol = new midi_event;
    vol->status = static_cast<EventType>(channel | EV_CONTROL);
    vol->data[0] = 7;

    if (!temp)
	vol->data[1] = 64;
    else
	vol->data[1] = temp->data[1];


    // Copy Bank
    if (bank && (bank->time > patch->time+PATCH_VOL_PAN_BIAS || bank->time < patch->time-PATCH_VOL_PAN_BIAS))
	bank = NULL;

    temp = bank;

    bank = new midi_event;
    bank->status = static_cast<EventType>(channel | EV_CONTROL);
    bank->data[0] = 0;

    if (!temp)
	bank->data[1] = 0;
    else
	bank->data[1] = temp->data[1];

    // Copy Pan
    if (pan && (pan->time > patch->time+PATCH_VOL_PAN_BIAS || pan->time < patch->time-PATCH_VOL_PAN_BIAS))
	pan = NULL;

    temp = pan;
    pan = new midi_event;
    pan->status = static_cast<EventType>(channel | EV_CONTROL);
    pan->data[0] = 10;

    if (!temp)
	pan->data[1] = 64;
    else
	pan->data[1] = temp->data[1];


    vol->time = 0;
    pan->time = 0;
    patch->time = 0;
    bank->time = 0;

    bank->next = vol;
    vol->next = pan;
    pan->next = patch;
    patch->next = list;
    list = bank;
}

// DuplicateAndMerge
void XmiFile::duplicateAndMerge(int32_t num)
{
    int32_t	i;
    std::vector<midi_event*>	track;
    int32_t	time = 0;
    int32_t	start = 0;
    int32_t	end = 1;

    if (info.type == 1) {
	start = 0;
	end = info.tracks;
    }
    else if (num >= 0 && num < info.tracks) {
	start += num;
	end += num;
    }

    track = _events;

    current = list = NULL;


    while (true)
    {
	int32_t	lowest = 1 << 30;
	int32_t	selected = -1;
	int32_t	num_na = end-start;

	// Firstly we find the track with the lowest time
	// for it's current event
	for (i = start; i < end; i++) {
	    if (!track[i]) {
		num_na--;
	    } else if (track[i]->time < lowest) {
		selected = i;
		lowest = track[i]->time;			
	    }
	}

	// This is just so I don't have to type [selected] all the time
	i = selected;

	// None left to convert
	if (!num_na) break;

	// Only need 1 end of track
	// So take the last one and ignore the rest;
	if ((num_na != 1) && (track[i]->status == EV_META) && (track[i]->data[0] == META_EOT))	{
	    track[i] = NULL;
	    continue;
	}

	if (current) {
	    current->next = new midi_event;
	    current = current->next;
	}
	else
	    list = current = new midi_event;

	current->next = NULL;

	time = track[i]->time;
	current->time = time;

	current->status = track[i]->status;
	current->data[0] = track[i]->data[0];
	current->data[1] = track[i]->data[1];

	if(!track[i]->buffer.empty())
	    current->buffer = track[i]->buffer;

	track[i] = track[i]->next;
    }
}


// Converts Events
//
// Source is at the first data byte
// size 1 is single data byte
// size 2 is dual data byte
// size 3 is XMI Note on
// Returns bytes converted

int32_t XmiFile::convertEvent(const int32_t time, const EventType status, const int32_t size)
{
    uint32_t	delta = 0;
    int32_t	data;
    IffChunk chunk = _iff.getChunk();

    data = chunk->get();


    // Bank changes are handled here
    if ((status & 0xf0) == EV_CONTROL && data == 0) {
	data = chunk->get();

	bank127[status&0xF] = false;

	createNewEvent (time);
	current->status = status;
	current->data[0] = 0;
	current->data[1] = data;


	return 2;
    }

    createNewEvent(time);
    current->status = status;

    current->data[0] = data;

    if (size == 1)
	return 1;

    current->data[1] = chunk->get();

    if (size == 2)
	return 2;

    // XMI Note On handling
    midi_event *prev = current;
    int32_t i = getVLQ(delta);
    createNewEvent(time+delta*3);

    current->status = status;
    current->data[0] = data;
    current->data[1] = 0;
    current = prev;

    return i + 2;
}

// Simple routine to convert system messages
int32_t XmiFile::convertSystemMessage(const int32_t time, const EventType status)
{
    int32_t i=0;
    uint32_t bufsiz;
    IffChunk chunk = _iff.getChunk();

    createNewEvent(time);
    current->status = status;

    // Handling of Meta events
    if (status == EV_META)
    {
	current->data[0] = chunk->get();
	i++;	
    }

    i += getVLQ(bufsiz);
    current->buffer.resize(bufsiz);

    if (current->buffer.empty()) return i;

    chunk->read(reinterpret_cast<char*>(&current->buffer.front()), current->buffer.size());

    return i+current->buffer.size();
}

// XMIDI and Midi to List
// Returns XMIDI PPQN
int32_t XmiFile::convertFiletoList(const bool is_xmi)
{
    int32_t 	time = 0;
    uint32_t 	data;
    bool	end = false;
    int32_t	tempo = 500000;
    int32_t	tempo_set = 0;
    EventType	status;
    int32_t	play_size = 2;

    IffChunk chunk = _iff.getChunk();

    if (is_xmi) play_size = 3;

    while (!end && !chunk->eof()) {

	if (!is_xmi) {
	    getVLQ(data);
	    time += data;

	    data = chunk->get();

	    if (data >= EV_NOTE_OFF) {
		status = static_cast<EventType>(data);
	    }
	    else
		chunk->seekg(-1, std::ios::cur);
	}	
	else {
	    getVLQ2 (data);
	    time += data*3;

	    status = static_cast<EventType>(chunk->get());
	}

	switch (status & 0xf0) {
	    case EV_NOTE_ON:
		convertEvent(time, status, play_size);
		break;

		// 2 byte data
	    case EV_NOTE_OFF:
	    case EV_POLY_PRESS:
	    case EV_CONTROL:
	    case EV_PITCH:
		convertEvent(time, status, 2);
		break;


		// 1 byte data
	    case EV_PROG:
	    case EV_CHAN_PRES:
		convertEvent(time, status, 1);
		break;


	    case EV_SYSEX:
		if (status == EV_META) {
		    std::streampos	pos = chunk->tellg();
		    uint32_t		data = chunk->get();

		    if (data == META_EOT)
			end = true;
		    else if (data == META_TEMPO && !tempo_set) { // Tempo. Need it for PPQN
			tempo = chunk->get() * ((chunk->get()<<16) | chunk->getU16BE());
			tempo_set = 1;
		    } else if (data == META_TEMPO && tempo_set && is_xmi) {// Skip any other tempo changes
			getVLQ(data);
			chunk->ignore(data);
			break;
		    }

		    chunk->seekg(pos);
		}
		convertSystemMessage(time, status);
		break;

	    default:
		break;
	}

    }
    return (tempo*3)/25000;
}

uint32_t XmiFile::convertListToMTrk (OStream &dest, midi_event *mlist)
{
    int32_t	time = 0;
    midi_event	*event;
    uint32_t	delta;
    uint8_t	last_status = 0;
    uint32_t 	i = 8;
    bool	end = false;

    dest.put('M');
    dest.put('T');
    dest.put('r');
    dest.put('k');

    std::streampos size_pos = dest.tellp();
    dest.putU32LE(0);

    int32_t x = 0;
    for (event = mlist; event && !end; event=event->next, x++) {
	delta = (event->time - time);
	time = event->time;

	i += putVLQ(dest, delta);

	if ((event->status != last_status) || (event->status >= EV_SYSEX)) {
	    dest.put(event->status);
	    i++;
	}

	last_status = event->status;

	switch (event->status & 0xf0) {
	    // 2 bytes data
	    case EV_NOTE_OFF:
	    case EV_NOTE_ON:
	    case EV_POLY_PRESS:
	    case EV_CONTROL:
	    case EV_PITCH:
		dest.put(event->data[0]);
		dest.put(event->data[1]);
		i += 2;
		break;


		// 1 bytes data
	    case EV_PROG:
	    case EV_CHAN_PRES:
		dest.put(event->data[0]);
		i++;
		break;


		// Variable length
	    case EV_SYSEX:
		if (event->status == EV_META) {
		    if (event->data[0] == META_EOT) end = true;
		    dest.put(event->data[0]);
		    i++;
		}

		i += putVLQ(dest, event->buffer.size());

		if (!event->buffer.empty()) {
		    for (std::vector<uint8_t>::iterator it = event->buffer.begin(); it != event->buffer.end(); ++it)
			dest.put(*it);
		    i += event->buffer.size();
		}

		break;


		// Never occur
	    default:
		//cerr << "Not supposed to see this" << endl;
		break;
	}
    }

    std::streampos cur_pos = dest.tellp();
    dest.seekp(size_pos);
    dest.putU32BE(i-8);
    dest.seekp(cur_pos);
    return i;
}

// Assumes correct xmidi
int32_t XmiFile::extractTracksFromXmi()
{
    int32_t	num = 0;
    int16_t	ppqn;
    for(IffChunk chunk = _iff.getChunk(); chunk && num != info.tracks; chunk = _iff.next()) {
	if(chunk->id != ID_EVNT) {
	    continue;
	}

	list = NULL;

	// Convert it
	if (!(ppqn = convertFiletoList(true))) {
	    //cerr << "Unable to convert data" << endl;
	    break;
	}
	timing[num] = ppqn;
	_events[num] = list;

	// Increment Counter
	num++;
    }


    // Return how many were converted
    return num;
}

#if 0
int32_t XmiFile::extractTracksFromMid (IStream &source)
{
    int32_t	num = 0;
    uint32_t	len = 0;
    char	buf[32];

    while (!source.eof() && num != info.tracks)
    {
	// Read first 4 bytes of name
	source.read (buf, 4);
	len = source.getU32BE();

	if (memcmp(buf,"MTrk",4))
	{
	    source.ignore (len);
	    continue;
	}

	list = NULL;
	int32_t begin = static_cast<uint32_t>(source.tellg());

	// Convert it
	if (!ConvertFiletoList (source, false))
	{
	    //cerr << "Unable to convert data" << endl;
	    break;
	}

	events[num] = list;

	// Increment Counter
	num++;		
	source.seekg (begin+len);
    }


    // Return how many were converted
    return num;
}
#endif

int32_t XmiFile::extractTracks()
{
    int32_t 		count;

    IffChunk		chunk = _iff.getChunk();
    // Could be XMIDI
    if(_iff.getType() == ID_FORM) {

	// XDIRless XMIDI, we can handle them here.
	if (_iff.getGroupType() == ID_XMID) {	
	    //cerr << "Warning: XMIDI doesn't have XDIR" << endl;
	    info.tracks = 1;

	} // Not an XMIDI that we recognise
	else if (_iff.getGroupType() != ID_XDIR)
	    throw(Exception(LOG_ERROR, "XmiFile::extractTracks()", "Not a recognised XMID, expected 'XDIR' or 'XMID' section, got '%s'", ID2string(_iff.getGroupType()).c_str()));
	else {
	    info.tracks = 0;


	    if(chunk->id == ID_INFO)
		info.tracks = chunk->getU16LE();
	    else
    		throw(Exception(LOG_ERROR, "XmiFile::extractTracks()", "Expected 'INFO' section, got '%s'", static_cast<std::string>(*chunk).c_str()));

	    chunk = _iff.next();

	    if(_iff.getType(-2) != ID_CAT)
    		throw(Exception(LOG_ERROR, "XmiFile::extractTracks()", "Not a recognised XMID, expected 'CAT ' section, got '%s'", ID2string(_iff.getType(-2)).c_str()));

	    if(_iff.getGroupType() != ID_XMID)
    		throw(Exception(LOG_ERROR, "XmiFile::extractTracks()", "Chunk id '%s' != 'XMID'", ID2string(_iff.getGroupType()).c_str()));

	}

	// Ok it's an XMID, so pass it to the ExtractCode

	_events.resize(info.tracks);// = new midi_event *[info.tracks];
	timing.resize(info.tracks);
	fixed.resize(info.tracks);
	info.type = 0;

	count = extractTracksFromXmi();

	if (count != info.tracks)
	{
	    //cerr << "Error: unable to extract all (" << info.tracks << ") tracks specified from XMIDI. Only ("<< count << ")" << endl;

	    int32_t i = 0;


	    return 0;		
	}

	return 1;

    }
    //TODO: add support for this later..?
#if 0 
    // Definately a Midi
    else if (!memcmp (buf, "MThd", 4))
    {
	// Simple read length of header
	len = source.getU32BE();

	if (len < 6)
	{
	    //cerr << "Not a valid MIDI" << endl;
	    return 0;
	}

	info.type = source.getU16BE();

	info.tracks = source.getU16BE();

	events = new midi_event *[info.tracks];
	timing.resize(info.tracks);
	fixed.resize(info.tracks);
	timing[0] = source.getU16BE();
	for (i = 0; i < info.tracks; i++)
	{
	    timing[i] = timing[0];
	    events[i] = NULL;
	    fixed[i] = false;
	}

	count = extractTracksFromMid (source);

	if (count != info.tracks)
	{
	    //cerr << "Error: unable to extract all (" << info.tracks << ") tracks specified from MIDI. Only ("<< count << ")" << endl;

	    for (i = 0; i < info.tracks; i++)
		deleteEventList (events[i]);

	    delete [] events;

	    return 0;

	}

	return 1;

    }// A RIFF Midi, just pass the source back to this function at the start of the midi file
    else if (!memcmp (buf, "RIFF", 4))
    {
	// Read len
	len = source.getU32LE();

	// Read 4 bytes of type
	source.read (buf, 4);

	// Not an RMID
	if (memcmp (buf, "RMID", 4))
	{
	    //cerr << "Invalid RMID" << endl;
	    return 0;
	}

	// Is a RMID

	for (i = 4; i < len; i++)
	{
	    // Read 4 bytes of type
	    source.read (buf, 4);

	    chunk_len = source.getU32LE();

	    i+=8;

	    if (memcmp (buf, "data", 4))
	    {	
		// Must allign
		source.ignore ((chunk_len+1)&~1);
		i+= (chunk_len+1)&~1;
		continue;
	    }

	    return extractTracks (source);

	}

	//cerr << "Failed to find midi data in RIFF Midi" << endl;
	return 0;
    }
#endif

    return 0;	
}

}
