/*
    util.c - misc utilities
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

#include "common.h"

// SDCC 3.4 has no memcpy for stm8
void *
memcpy (void *dst, const void *src, size_t acount)
{
  char *d = (char *)dst;
  const char *s = (const char *)src;

  while (acount--)
    *d++ = *s++;

  return dst;
}


// divmod macro integer or (decimal)fixedpoint
// n must be a var
#define DEC_DIG_NEXT(d,n) {d = '0' + (n % 10); n /= 10;}

/** 
  * convert uint32_t to string
  * @param[out] *s output string, will not be null terminated
  * @param[in]   d integer input
  * @param[in]   l output length (including ".")
  * @param[in]   f output fraction length, 0 for integer
  * @param[in]   neg negative sign
  * 
  */
void sprint_u32_fxp(char *s, uint32_t d, int8_t l, int8_t f, bool neg) {
//  bool neg = false;
  int8_t k;
  int8_t li;
  int8_t p = l - 1;
//   if (d < 0) {
//     d = -d;
//     neg = true;
//   }
  if (f > 0) {
    for (k = 0; k < f; k++) { // fraction
      DEC_DIG_NEXT(s[p--], d);
    }
    s[p--] = '.';
    li = l - f - 2;
  } else {
    li = l - 1;
  }
  if (0 == d) {
    s[p--] = '0';
  } else {
    for (k = 0; k < li && 0 != d; k++) { // integer
      DEC_DIG_NEXT(s[p--], d);
    }
  }
  if (neg) { // sign
    s[p--] = '-';
  }
  while (p >= 0) { // fill space
    s[p--] = ' ';
  }
  //  s[l] = 0;
}

