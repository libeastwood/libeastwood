/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2005 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * diskopl.cpp - Disk Writer OPL, by Simon Peter <dn.tlp@gmx.net>
 */

#include "StdDef.h"
#include "adl/diskopl.h"

namespace eastwood {

//static const unsigned short note_table[12] = {363,385,408,432,458,485,514,544,577,611,647,686};
const unsigned char CDiskopl::op_table[9] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};

CDiskopl::CDiskopl(const std::ostream &stream) :
  _stream(const_cast<OStream&>(reinterpret_cast<const OStream&>(stream))),
  old_freq(0.0f), del(1), nowrite(false)
{
  unsigned short clock = 0xffff;

  currType = TYPE_OPL3;
  _stream.write("RAWADATA",8);
  _stream.write((char*)&clock, sizeof(clock));
}

CDiskopl::~CDiskopl()
{
}

void CDiskopl::update(CadlPlayer *p)
{
  unsigned short	clock;
  unsigned int		wait;

  if(p->getrefresh() != old_freq) {
    old_freq = p->getrefresh();
    del = wait = (unsigned int)(18.2f / old_freq);
    clock = (unsigned short)(1192737/(old_freq*(wait+1)));
    _stream.put(0); _stream.put(2);
    _stream.write((char*)&clock, sizeof(clock));
  }
  if(!nowrite) {
    _stream.put(del+1);
    _stream.put(0);
  }
}

void CDiskopl::setchip(int n)
{
  Copl::setchip(n);

  if(!nowrite) {
    _stream.put(currChip + 1);
    _stream.put(2);
  }
}

void CDiskopl::write(int reg, int val)
{
  if(!nowrite)
    diskwrite(reg,val);
}

void CDiskopl::init()
{
  for (int i=0;i<9;i++) {	// stop instruments
    diskwrite(0xb0 + i,0);		// key off
    diskwrite(0x80 + op_table[i],0xff);	// fastest release
  }
  diskwrite(0xbd,0);	// clear misc. register
}

void CDiskopl::diskwrite(int reg, int val)
{
  _stream.put(val);
  _stream.put(reg);
}

}
