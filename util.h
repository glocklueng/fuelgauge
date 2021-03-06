/*
    util.h - misc utilities
    Copyright (C) 2012-2016  Terrance Hendrik terrance.hendrik@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _UTIL_H
#define _UTIL_H

void sprint_u32_fxp(char *s, uint32_t d, int8_t l, int8_t f, bool neg);
void *memcpy(void *dst,const void *src,size_t acount);

void divmod_u16_u8(uint16_t *a_q, uint8_t *b_r);
void divmod_u8_u8(uint8_t *a_q, uint8_t *b_r);

#endif
