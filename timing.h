/*
    timing.h - STM8 time keeping and button scan
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


#ifndef _TIMING_H
#define _TIMING_H

#define BTN_T_GLITCH 30  // 60 ms
#define BTN_T_HOLD 1000  // 2 sec
#define BTN_PORT PORTD
#define BTN_PIN 1
#define BTN_PIN_MSK (1<<BTN_PIN)

#define BTN_DOWN     1
#define BTN_EVENT    0x80
#define BTN_HOLD     2
#define BTN_UP       0

void delayms(uint32_t t);
void delayus(uint32_t t);
void isr_t4()__interrupt(23);
void init_tick();

extern volatile uint16_t btn_tmr;
extern volatile uint8_t btn_stat;
extern volatile uint32_t tick_sec;
extern volatile uint16_t tick_2ms;

#endif
