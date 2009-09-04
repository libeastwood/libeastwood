/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2003 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file contains an example implementation of the Scale effect
 * applyed to a generic bitmap.
 *
 * You can find an high level description of the effect at :
 *
 * http://scale2x.sourceforge.net/
 *
 * Alternatively at the previous license terms, you are allowed to use this
 * code in your program with these conditions:
 * - the program is not used in commercial activities.
 * - the whole source code of the program is released with the binary.
 * - derivative works of the program are allowed.
 */

#ifndef EASTWOOD_SCALEBIT_H
#define EASTWOOD_SCALEBIT_H

#include "Surface.h"

namespace eastwood {

/**
 * Check if the scale implementation is applicable at the given arguments.
 * \param scale Scale factor. Scale2X, Scale2X3, Scale2X4, Scale3X, Scale4X.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 * \return
 *   - false on precondition violated.
 *   - true on success.
 */
bool scale_precondition(Scaler scale, uint8_t bpp, uint16_t width, uint16_t height);

/**
 * Apply the Scale effect on a bitmap.
 * This function is simply a common interface for ::scale2x(), ::scale3x() and ::scale4x().
 * \param scale Scale factor. Scale2X, Scale2X3, Scale2X4, Scale3X, Scale4X.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
void scale(Scaler scale, uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height);

}

#endif // EASTWOOD_SCALEBIT_H
