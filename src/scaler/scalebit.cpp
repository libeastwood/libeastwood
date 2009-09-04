/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2003, 2004 Andrea Mazzoleni
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

#include <vector>

#include "StdDef.h"

#include "scalebit.h"

#include "scale2x.h"
#include "scale3x.h"

#include <cassert>

#define SSDST(bits, num) (uint##bits##_t *)dst##num
#define SSSRC(bits, num) (const uint##bits##_t *)src##num

/**
 * Apply the Scale2x effect on a group of rows. Used internally.
 */
static inline void stage_scale2x(uint8_t* dst0, uint8_t* dst1, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint8_t bpp, uint32_t bpp_per_row)
{
	switch (bpp) {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
		case 1 : scale2x_8_mmx(SSDST(8,0), SSDST(8,1), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x_16_mmx(SSDST(16,0), SSDST(16,1), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x_32_mmx(SSDST(32,0), SSDST(32,1), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#elif defined(__arm__)
		//TODO:
		case 1 : scale2x_8_arm(DST(8,0), DST(8,1), SRC(8,0), SRC(8,1), SRC(8,2), bpp_per_row); break;
		case 2 : scale2x_16_arm(DST(16,0), DST(16,1), SRC(16,0), SRC(16,1), SRC(16,2), bpp_per_row); break;
		case 4 : scale2x_32_arm(DST(32,0), DST(32,1), SRC(32,0), SRC(32,1), SRC(32,2), bpp_per_row); break;
#else
		case 1 : scale2x_8_def(SSDST(8,0), SSDST(8,1), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x_16_def(SSDST(16,0), SSDST(16,1), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x_32_def(SSDST(32,0), SSDST(32,1), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#endif
	}
}

/**
 * Apply the Scale2x3 effect on a group of rows. Used internally.
 */
static inline void stage_scale2x3(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint8_t bpp, uint32_t bpp_per_row)
{
	switch (bpp) {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
		case 1 : scale2x3_8_mmx(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x3_16_mmx(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x3_32_mmx(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#else
		case 1 : scale2x3_8_def(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x3_16_def(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x3_32_def(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#endif
	}
}

/**
 * Apply the Scale2x4 effect on a group of rows. Used internally.
 */
static inline void stage_scale2x4(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, uint8_t* dst3, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint8_t bpp, uint32_t bpp_per_row)
{
	switch (bpp) {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
		case 1 : scale2x4_8_mmx(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSDST(8,3), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x4_16_mmx(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSDST(16,3), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x4_32_mmx(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSDST(32,3), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#else
		case 1 : scale2x4_8_def(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSDST(8,3), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale2x4_16_def(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSDST(16,3), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale2x4_32_def(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSDST(32,3), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
#endif
	}
}

/**
 * Apply the Scale3x effect on a group of rows. Used internally.
 */
static inline void stage_scale3x(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint8_t bpp, uint32_t bpp_per_row)
{
	switch (bpp) {
		case 1 : scale3x_8_def(SSDST(8,0), SSDST(8,1), SSDST(8,2), SSSRC(8,0), SSSRC(8,1), SSSRC(8,2), bpp_per_row); break;
		case 2 : scale3x_16_def(SSDST(16,0), SSDST(16,1), SSDST(16,2), SSSRC(16,0), SSSRC(16,1), SSSRC(16,2), bpp_per_row); break;
		case 4 : scale3x_32_def(SSDST(32,0), SSDST(32,1), SSDST(32,2), SSSRC(32,0), SSSRC(32,1), SSSRC(32,2), bpp_per_row); break;
	}
}

/**
 * Apply the Scale4x effect on a group of rows. Used internally.
 */
static inline void stage_scale4x(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, uint8_t* dst3, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, const uint8_t* src3, uint8_t bpp, uint32_t bpp_per_row)
{
	stage_scale2x(dst0, dst1, src0, src1, src2, bpp, 2 * bpp_per_row);
	stage_scale2x(dst2, dst3, src1, src2, src3, bpp, 2 * bpp_per_row);
}

#define SCDST(i) (dst+(i)*dstSlice)
#define SCSRC(i) (src+(i)*srcSlice)
#define SCMID(i) (mid[(i)])

/**
 * Apply the Scale2x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x2 times the size of the source bitmap.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale2x(uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint8_t* dst = (uint8_t*)dstPtr;
	const uint8_t* src = (const uint8_t*)srcPtr;
	uint16_t count;

	assert(height >= 2);

	count = height;

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(0), SCSRC(1), bpp, width);

	dst = SCDST(2);

	count -= 2;
	while (count) {
		stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(2), bpp, width);

		dst = SCDST(2);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(1), bpp, width);

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale2x3 effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x3 times the size of the source bitmap.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale2x3(uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint8_t* dst = (uint8_t*)dstPtr;
	const uint8_t* src = (const uint8_t*)srcPtr;
	uint16_t count;

	assert(height >= 2);

	count = height;

	stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(0), SCSRC(1), bpp, width);

	dst = SCDST(3);

	count -= 2;
	while (count) {
		stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(2), bpp, width);

		dst = SCDST(3);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x3(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(1), bpp, width);

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale2x4 effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x4 times the size of the source bitmap.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale2x4(uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint8_t* dst = (uint8_t*)dstPtr;
	const uint8_t* src = (const uint8_t*)srcPtr;
	uint16_t count;

	assert(height >= 2);

	count = height;

	stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(0), SCSRC(1), bpp, width);

	dst = SCDST(4);

	count -= 2;
	while (count) {
		stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(1), SCSRC(2), bpp, width);

		dst = SCDST(4);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x4(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCSRC(0), SCSRC(1), SCSRC(1), bpp, width);

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale3x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 3x3 times the size of the source bitmap.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale3x(uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint8_t* dst = (uint8_t*)dstPtr;
	const uint8_t* src = (const uint8_t*)srcPtr;
	uint16_t count;

	assert(height >= 2);

	count = height;

	stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(0), SCSRC(1), bpp, width);

	dst = SCDST(3);

	count -= 2;
	while (count) {
		stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(2), bpp, width);

		dst = SCDST(3);
		src = SCSRC(1);

		--count;
	}

	stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(1), bpp, width);
}

/**
 * Apply the Scale4x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 4x4 times the size of the source bitmap.
 * \note This function requires also a small buffer bitmap used internally to store
 * intermediate results. This bitmap must have at least an horizontal size in bytes of 2*width*bpp,
 * and a vertical size of 6 rows. The memory of this buffer must not be allocated
 * in video memory because it's also read and not only written. Generally
 * a heap (malloc) or a stack (alloca) buffer is the best choice.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param void_mid Pointer at the first bpp of the buffer bitmap.
 * \param mid_slice Size in bytes of a buffer bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale4x_buf(uint8_t* dstPtr, uint16_t dstSlice, uint8_t* void_mid, uint16_t mid_slice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint8_t* dst = (uint8_t*)dstPtr;
	const uint8_t* src = (const uint8_t*)srcPtr;
	uint16_t count;
	uint8_t* mid[6];

	assert(height >= 4);

	count = height;

	/* set the 6 buffer pointers */
	mid[0] = (uint8_t*)void_mid;
	mid[1] = mid[0] + mid_slice;
	mid[2] = mid[1] + mid_slice;
	mid[3] = mid[2] + mid_slice;
	mid[4] = mid[3] + mid_slice;
	mid[5] = mid[4] + mid_slice;

	stage_scale2x(SCMID(-2+6), SCMID(-1+6), SCSRC(0), SCSRC(0), SCSRC(1), bpp, width);
	stage_scale2x(SCMID(0), SCMID(1), SCSRC(0), SCSRC(1), SCSRC(2), bpp, width);
	stage_scale2x(SCMID(2), SCMID(3), SCSRC(1), SCSRC(2), SCSRC(3), bpp, width);
	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(-2+6), SCMID(-2+6), SCMID(-1+6), SCMID(0), bpp, width);

	dst = SCDST(4);

	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(-1+6), SCMID(0), SCMID(1), SCMID(2), bpp, width);

	dst = SCDST(4);

	count -= 4;
	while (count) {
		uint8_t* tmp;

		stage_scale2x(SCMID(4), SCMID(5), SCSRC(2), SCSRC(3), SCSRC(4), bpp, width);
		stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(1), SCMID(2), SCMID(3), SCMID(4), bpp, width);

		dst = SCDST(4);
		src = SCSRC(1);

		tmp = SCMID(0); /* shift by 2 position */
		SCMID(0) = SCMID(2);
		SCMID(2) = SCMID(4);
		SCMID(4) = tmp;
		tmp = SCMID(1);
		SCMID(1) = SCMID(3);
		SCMID(3) = SCMID(5);
		SCMID(5) = tmp;

		--count;
	}

	stage_scale2x(SCMID(4), SCMID(5), SCSRC(2), SCSRC(3), SCSRC(3), bpp, width);
	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(1), SCMID(2), SCMID(3), SCMID(4), bpp, width);

	dst = SCDST(4);

	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(3), SCMID(4), SCMID(5), SCMID(5), bpp, width);

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale4x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 4x4 times the size of the source bitmap.
 * \note This function operates like ::scale4x_buf() but the intermediate buffer is
 * automatically allocated in the stack.
 * \param dstPtr Pointer at the first bpp of the destination bitmap.
 * \param dstSlice Size in bytes of a destination bitmap row.
 * \param srcPtr Pointer at the first bpp of the source bitmap.
 * \param srcSlice Size in bytes of a source bitmap row.
 * \param bpp Bytes per bpp of the source and destination bitmap.
 * \param width Horizontal size in bpps of the source bitmap.
 * \param height Vertical size in bpps of the source bitmap.
 */
static void scale4x(uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
	uint16_t mid_slice;
	std::vector<uint8_t> mid;

	mid_slice = 2 * bpp * width; /* required space for 1 row buffer */

	mid_slice = (mid_slice + 0x7) & ~0x7; /* align to 8 bytes */

	mid.resize(6 * mid_slice); /* allocate space for 6 row buffers */

	scale4x_buf(dstPtr, dstSlice, &mid.front(), mid_slice, srcPtr, srcSlice, bpp, width, height);

}

bool scale_precondition(Scaler scale, uint8_t bpp, uint16_t width, uint16_t height)
{
	if (bpp != 1 && bpp != 2 && bpp != 4)
		return false;

	switch (scale) {
	case Scale2X:
	case Scale2X3:
	case Scale2X4:
	case Scale3X:
		if (height < 2)
			return false;
		break;
	case Scale4X:
		if (height < 4)
			return false;
		break;
	default:
		return false;
	}

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	switch (scale) {
	case Scale2X:
	case Scale2X3:
	case Scale2X4:

	case Scale4X:
		if (width < (16 / bpp))
			return false;
		if (width % (8 / bpp) != 0)
			return false;
		break;
	case Scale3X:
		if (width < 2)
			return false;
		break;
	}
#else
	if (width < 2)
		return false;
#endif
	return true;
}

void scale(Scaler scale, uint8_t* dstPtr, uint16_t dstSlice, const uint8_t* srcPtr, uint16_t srcSlice, uint8_t bpp, uint16_t width, uint16_t height)
{
    switch (scale) {
	case Scale2X:
	    scale2x(dstPtr, dstSlice, srcPtr, srcSlice, bpp, width, height);
	    break;
	case Scale2X3:
	    scale2x3(dstPtr, dstSlice, srcPtr, srcSlice, bpp, width, height);
	    break;
	case Scale2X4:
	    scale2x4(dstPtr, dstSlice, srcPtr, srcSlice, bpp, width, height);
	    break;
	case Scale3X:
	    scale3x(dstPtr, dstSlice, srcPtr, srcSlice, bpp, width, height);
	    break;
	case Scale4X:
	    scale4x(dstPtr, dstSlice, srcPtr, srcSlice, bpp, width, height);
	    break;
    }
}

