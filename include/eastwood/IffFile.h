/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/common/iff_container.h $
 * $Id: iff_container.h 41474 2009-06-12 10:26:58Z peres001 $
 */

#ifndef EASTWOOD_IFFCONTAINER_H
#define EASTWOOD_IFFCONTAINER_H

#include <vector>

#include "eastwood/StdDef.h"
#include "eastwood/IStream.h"
#include "eastwood/Exception.h"

namespace eastwood {

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define MKID_BE(a,b,c,d) (static_cast<uint32_t>((d | (c << 8) | (b << 16) | (a << 24))))
#else
    #define MKID_BE(a,b,c,d) (static_cast<uint32_t>((a | (b << 8) | (c << 16) | (d << 24))))
#endif

enum IFF_ID {
    /* Amiga 8 bits voice */
    ID_FORM	= MKID_BE('F','O','R','M'),
    /* EA IFF 85 group identifier */
    ID_CAT	= MKID_BE('C','A','T',' '),
    /* EA IFF 85 group identifier */
    ID_LIST	= MKID_BE('L','I','S','T'),
    /* EA IFF 85 group identifier */
    ID_PROP	= MKID_BE('P','R','O','P'),
    /* EA IFF 85 group identifier */
    ID_END	= MKID_BE('E','N','D',' '),
    /* unofficial END-of-FORM identifier (see Amiga RKM Devices Ed.3
       page 376) */
    ID_ILBM	= MKID_BE('I','L','B','M'),
    /* EA IFF 85 raster bitmap form */
    ID_DEEP	= MKID_BE('D','E','E','P'),
    /* Chunky pixel image files (Used in TV Paint) */
    ID_RGB8	= MKID_BE('R','G','B','8'),
    /* RGB image forms, Turbo Silver (Impulse) */
    ID_RGBN	= MKID_BE('R','G','B','N'),
    /* RGB image forms, Turbo Silver (Impulse) */
    ID_PBM	= MKID_BE('P','B','M',' '),
    /* 256-color chunky format (DPaint 2 ?) */
    ID_ACBM	= MKID_BE('A','C','B','M'),
    /* Amiga Contiguous Bitmap (AmigaBasic) */
    ID_8SVX	= MKID_BE('8','S','V','X'),
    /* Amiga 8 bits voice */

/* generic */

    ID_FVER	= MKID_BE('F','V','E','R'),
/* AmigaOS version string */
    ID_JUNK	= MKID_BE('J','U','N','K'),
/* always ignore this chunk */
    ID_ANNO	= MKID_BE('A','N','N','O'),
/* EA IFF 85 Generic Annotation chunk */
    ID_AUTH	= MKID_BE('A','U','T','H'),
/* EA IFF 85 Generic Author chunk */
    ID_CHRS	= MKID_BE('C','H','R','S'),
/* EA IFF 85 Generic character string chunk */
    ID_NAME	= MKID_BE('N','A','M','E'),
/* EA IFF 85 Generic Name of art, music, etc. chunk */
    ID_TEXT	= MKID_BE('T','E','X','T'),
/* EA IFF 85 Generic unformatted ASCII text chunk */
    ID_copy	= MKID_BE('(','c',')',' '),
/* EA IFF 85 Generic Copyright text chunk */

/* ILBM chunks */

    ID_BMHD	= MKID_BE('B','M','H','D'),
/* ILBM BitmapHeader */
    ID_CMAP	= MKID_BE('C','M','A','P'),
/* ILBM 8bit RGB colormap */
    ID_GRAB	= MKID_BE('G','R','A','B'),
/* ILBM "hotspot" coordiantes */
    ID_DEST	= MKID_BE('D','E','S','T'),
/* ILBM destination image info */
    ID_SPRT	= MKID_BE('S','P','R','T'),
/* ILBM sprite identifier */
    ID_CAMG	= MKID_BE('C','A','M','G'),
/* Amiga viewportmodes */
    ID_BODY	= MKID_BE('B','O','D','Y'),
/* ILBM image data */
    ID_CRNG	= MKID_BE('C','R','N','G'),
/* color cycling */
    ID_CCRT	= MKID_BE('C','C','R','T'),
/* color cycling */
    ID_CLUT	= MKID_BE('C','L','U','T'),
/* Color Lookup Table chunk */
    ID_DPI 	= MKID_BE('D','P','I',' '),
/* Dots per inch chunk */
    ID_DPPV	= MKID_BE('D','P','P','V'),
/* DPaint perspective chunk (EA) */
    ID_DRNG	= MKID_BE('D','R','N','G'),
/* DPaint IV enhanced color cycle chunk (EA) */
    ID_EPSF	= MKID_BE('E','P','S','F'),
/* Encapsulated Postscript chunk */
    ID_CMYK	= MKID_BE('C','M','Y','K'),
/* Cyan, Magenta, Yellow, & Black color map (Soft-Logik) */
    ID_CNAM	= MKID_BE('C','N','A','M'),
/* Color naming chunk (Soft-Logik) */
    ID_PCHG	= MKID_BE('P','C','H','G'),
/* Line by line palette control information (Sebastiano Vigna) */
    ID_PRVW	= MKID_BE('P','R','V','W'),
/* A mini duplicate ILBM used for preview (Gary Bonham) */
    ID_XBMI	= MKID_BE('X','B','M','I'),
/* eXtended BitMap Information (Soft-Logik) */
    ID_CTBL	= MKID_BE('C','T','B','L'),
/* Newtek Dynamic Ham color chunk */
    ID_DYCP	= MKID_BE('D','Y','C','P'),
/* Newtek Dynamic Ham chunk */
    ID_SHAM	= MKID_BE('S','H','A','M'),
/* Sliced HAM color chunk */
    ID_ABIT	= MKID_BE('A','B','I','T'),
/* ACBM body chunk */
    ID_DCOL	= MKID_BE('D','C','O','L'),
/* unofficial direct color */
    ID_DPPS	= MKID_BE('D','P','P','S'),
/* ? */
    ID_TINY	= MKID_BE('T','I','N','Y'),
/* ? */

/* 8SVX chunks */

    ID_VHDR	= MKID_BE('V','H','D','R'),
/* 8SVX Voice8Header */

    ID_DATA	= MKID_BE('D','A','T','A'),
    ID_DESC	= MKID_BE('D','E','S','C'),
    ID_EMC2	= MKID_BE('E','M','C','2'),
    ID_EVNT	= MKID_BE('E','V','N','T'),
    ID_ICON	= MKID_BE('I','C','O','N'),
    ID_INFO	= MKID_BE('I','N','F','O'),
    ID_MENT	= MKID_BE('M','E','N','T'),
    ID_ORDR	= MKID_BE('O','R','D','R'),
    ID_RTBL	= MKID_BE('R','T','B','L'),
    ID_RPAL	= MKID_BE('R','P','A','L'),
    ID_SSET	= MKID_BE('S','S','E','T'),
    ID_SINF	= MKID_BE('S','I','N','F'),
    ID_TIMB	= MKID_BE('T','I','M','B'),    
    ID_XDIR	= MKID_BE('X','D','I','R'),    
    ID_XMID	= MKID_BE('X','M','I','D'),
    ID_FILLER	= MKID_BE(0,0,0,0)
};

std::string ID2string(IFF_ID id);


/**
 *  Represents a IFF chunk available to client code.
 *
 */
struct IFFChunk : public IStream {
	IFF_ID		id;
	uint32_t	size;

	IFFChunk() : id(ID_FILLER), size(0) {}
	IFFChunk(IStream &stream);
	IFFChunk(uint32_t id, uint32_t size, IStream &stream);
	virtual ~IFFChunk();

	operator std::string() const {
	    return ID2string(id);
	}
};

typedef std::tr1::shared_ptr<IFFChunk> IffChunk;

struct IFFGroupChunk : public IFFChunk {
    IFF_ID		groupId;

    IFFGroupChunk(uint32_t id, IStream &stream);
};

typedef std::tr1::shared_ptr<IFFGroupChunk> GroupChunk;

/**
 *  Parser for IFF containers.
 */
class IffFile
{

    protected:
	IStream				&_stream;
	std::vector<GroupChunk>		_formChunk;	//!< The root chunk of the file.
	IffChunk			_chunk; 	//!< The current chunk.

    public:
	IffFile(std::istream &stream);
	~IffFile();

	IffChunk next();

	IffChunk getChunk() { return _chunk; }

	IFF_ID getType(int32_t level = -1) const { return _formChunk.at(level < 0 ? _formChunk.size() + level : level)->id; }
	/**
	 * Returns the IFF FORM type.
	 * @return the IFF FORM type of the stream, or 0 if FORM header is not found.
	 */
	IFF_ID getGroupType(int32_t level = -1) const { return _formChunk.at(level < 0 ? _formChunk.size() + level : level)->groupId; }

	/**
	 * Returns the size of the data.
	 * @return the size of the data in file, or -1 if FORM header is not found.
	 */
	uint32_t getGroupSize(int32_t level = -1) const { return _formChunk.at(level < 0 ? _formChunk.size() + level : level)->size; }

	uint32_t getLevels() const throw() { return _formChunk.size(); }

};


}

#endif
