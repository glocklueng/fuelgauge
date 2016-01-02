/*
    timing.c - STM8 time keeping and button scan
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


#include "common.h"
#include "util.h"
#include "timing.h"

// for less CPU load, using 2ms tick
// 4ms tick system is even better, with uint8_t 4ms ticks
volatile
uint16_t tick_2ms = 0;

volatile
uint32_t tick_sec = 0;

volatile
uint8_t btn_stat = 0; //0: up, 1: down, 2: hold

volatile
uint16_t btn_tmr = 0;

// short microseconds delay, 
void delayus(uint32_t t){
    uint32_t c = t - 1;

    while(c--){
        ;
    }
}

// long miliseconds delay
void delayms(uint32_t t){
    uint32_t c = tick_sec * 1000 + tick_2ms * 2 + (t);
    
    while(c > (tick_sec * 1000 + tick_2ms * 2)){
        nop();// wait
    }
}

// 1-ms tick has CPU load 
// setup 2-ms tick, 16e6Hz/128/250=500Hz
// and 32-bit second ticker
void init_tick(){
    TIM4->EGR = 0x01; // update gen
    TIM4->PSCR = 0x07; // div 128
    TIM4->ARR = 250-1;
    TIM4->CNTR = 0;

    TIM4->IER = 0x01; // enable timer interrupt
    TIM4->CR1 = 0x01; // CEN
}


void isr_t4() __interrupt (23){ // interupt verctor 23
    static uint8_t btn_det = 0;
    TIM4->SR &= ~0x01; //  clear UIF flag

    tick_2ms++;
    if(tick_2ms >= 500){
        tick_2ms = 0;
        tick_sec++;
    }

    if (btn_det != (BTN_PORT->IDR & BTN_PIN_MSK)){
        // change or glitch
        btn_det = BTN_PORT->IDR & BTN_PIN_MSK;
        btn_tmr = 0;
        btn_stat = 0;
    }

    if(0 == (btn_det)){ // down
        if (btn_tmr <= BTN_T_HOLD){
            btn_tmr++;
            switch(btn_tmr){
                case BTN_T_GLITCH:
                    btn_stat = BTN_DOWN | BTN_EVENT;
                    break;
                case BTN_T_HOLD:
                    btn_stat = BTN_HOLD | BTN_EVENT;
                    break;
                default:
                    break;
            }
        }
    } else { // up
        if (btn_tmr <= BTN_T_GLITCH){
            btn_tmr++;
            if(btn_tmr == BTN_T_GLITCH) {
                btn_stat = BTN_UP | BTN_EVENT;
            }
        }
    }
}

