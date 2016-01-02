/*
    oled1306spi.c - SSD driver for SSD1306, SPI mode
    Copyright (C) 2015-2016  Terrance Hendrik terrance.hendrik@gmail.com

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

#ifndef _OLED_1306_SPI_H
#define _OLED_1306_SPI_H

#include <stdint.h>
#include <stdlib.h>
#include "stm8s.h"
#include "util.h"

void init_spi();
void oled_spi_end();
void oled_spi_begin(uint8_t cd);
void spi_write(uint8_t d);

void oled_write_buf_dat(const char *s, size_t n);
void oled_write_buf_cmd(const char *s, size_t n);
void oled_clr();
void oled_fill(uint8_t c);
void oled_printline(uint8_t line,const char *s);
void oled_on();
void oled_off();
void oled_print(const char *s);
void init_oled();

#endif // _OLED_1306_SPI_H

