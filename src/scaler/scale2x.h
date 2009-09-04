/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2001, 2002, 2003, 2004 Andrea Mazzoleni
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

#ifndef EASTWOOD_SCALER_SCALE2X_H
#define EASTWOOD_SCALER_SCALE2X_H

void scale2x_8_def(uint8_t* dst0, uint8_t* dst1, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x_16_def(uint16_t* dst0, uint16_t* dst1, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x_32_def(uint32_t* dst0, uint32_t* dst1, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

void scale2x3_8_def(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x3_16_def(uint16_t* dst0, uint16_t* dst1, uint16_t* dst2, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x3_32_def(uint32_t* dst0, uint32_t* dst1, uint32_t* dst2, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

void scale2x4_8_def(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, uint8_t* dst3, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x4_16_def(uint16_t* dst0, uint16_t* dst1, uint16_t* dst2, uint16_t* dst3, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x4_32_def(uint32_t* dst0, uint32_t* dst1, uint32_t* dst2, uint32_t* dst3, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

void scale2x_8_mmx(uint8_t* dst0, uint8_t* dst1, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x_16_mmx(uint16_t* dst0, uint16_t* dst1, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x_32_mmx(uint32_t* dst0, uint32_t* dst1, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

void scale2x3_8_mmx(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x3_16_mmx(uint16_t* dst0, uint16_t* dst1, uint16_t* dst2, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x3_32_mmx(uint32_t* dst0, uint32_t* dst1, uint32_t* dst2, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

void scale2x4_8_mmx(uint8_t* dst0, uint8_t* dst1, uint8_t* dst2, uint8_t* dst3, const uint8_t* src0, const uint8_t* src1, const uint8_t* src2, uint32_t count);
void scale2x4_16_mmx(uint16_t* dst0, uint16_t* dst1, uint16_t* dst2, uint16_t* dst3, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
void scale2x4_32_mmx(uint32_t* dst0, uint32_t* dst1, uint32_t* dst2, uint32_t* dst3, const uint32_t* src0, const uint32_t* src1, const uint32_t* src2, uint32_t count);

/**
 * End the use of the MMX instructions.
 * This function must be called before using any floating-point operations.
 */
static inline void scale2x_mmx_emms(void)
{
	__asm__ __volatile__ (
		"emms"
	);
}

#elif defined(__asm__)

extern "C" void scale2x_8_arm(scale2x_uint8* dst0, scale2x_uint8* dst1, const scale2x_uint8* src0, const scale2x_uint8* src1, const scale2x_uint8* src2, uint32_t count);
extern "C" void scale2x_16_arm(uint16_t* dst0, uint16_t* dst1, const uint16_t* src0, const uint16_t* src1, const uint16_t* src2, uint32_t count);
extern "C" void scale2x_32_arm(scale2x_uint32* dst0, scale2x_uint32* dst1, const scale2x_uint32* src0, const scale2x_uint32* src1, const scale2x_uint32* src2, uint32_t count);

#endif

#endif

