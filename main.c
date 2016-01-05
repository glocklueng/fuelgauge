/*
    main.c - STM8S003 USB OLED FuelGauge firmware
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

#include "oled1306spi.h"
#include "util.h"
#include "timing.h"

// buzzer, unconnected
#define BUZZ_PORT PORTD
#define BUZZ_MSK (1<<5)

// controllable pull-up 1MegOhm for sensing amplifier
// for better near zero current resolution
#define PULLUP_PORT PORTC
#define PULLUP_MSK (1<<3)

// precision:
// LDO (as voltage reference) 2%
// resistors 1~5%
// 
// 
// voltage divisor: 330k+100k, 4.3x
// 3300mV*4.3 = 14190 mV
// ((1000L<<8)/(1000.0*((1L<<10)-1)/14190L)) = 3547.5
// negative B for voltage is ok, but not for amperage 
#define K_VOLT 3625UL
#define B_VOLT 41

// sensing amplifier 18k/1k, gain=19
// input bias 3.3V * 1k/ 1001k = 3.3mV
// calculated B is -58
#define K_AMP 1952UL 
#define B_AMP -116

// oled line buffer
static
char buf_line[32];

// system settings
// [0, 1] for OLED
// other RFU
// EEPROM is written in words (4 bytes each)
uint8_t settings[4];

#define OFFSET_TICK         0
#define OFFSET_E            4
#define OFFSET_CAP          8
#define OFFSET_SETTINGS    12

#define AIN_CH_L 3
#define AIN_CH_H 4

// IIR filter, 0..16, to be processed as signed nuber, 0..15
// for IIR, >8 will result very long stable time
// or Rectangle window, 13 gives 6-bit more resolution and lower noise
// 13 -> 4 samples/sec
#define AIN_SMOOTH 13
// under voltage threashold 3V ~216, 3.6V ~260
// when VUSB drops slowly, VCC will also drop, readings will never 
// drop below ~250
#define AIN_UVP_TH 260

uint32_t buf_adc[2];

// when input voltage drops below threadhold, save data to EEPROM, and 
// wait for VUSB to recover
bool low_voltage_lockout = false;

// readings
uint16_t volt_mv;
uint16_t amp_ma;
uint16_t pwr_mw;
uint32_t e_mj;   // milijoule,   1/3600 mWh, 1/3600000 Wh
uint32_t cap_mc; // millicolumb, 1/3600 mAh, 1/3600000 Ah


uint8_t save_eeprom();
void load_eeprom();

void init_adc(){
    CLK->PCKENR2 |= 0x08; // clock enable

    ADC1->CSR = 0x00; // clr EOC
    ADC1->CR1 = 0x00; // turn off
    ADC1->CR1 = 0x20; // fADC = fMASTER/4, Single conversion mode
    ADC1->CR2 = 0x08; // right aligned
    ADC1->CR3 = 0x00; // Data buffer disabled 
}

// poll ADC readings, and filter
// returns 1 if results in buf_adc are valid
uint8_t poll_adc(void){
    static uint16_t count = 0;
    uint8_t ch;
    uint8_t i;
    uint16_t t;
    uint8_t r = 0;
    
    for (i = 0; i <= AIN_CH_H - AIN_CH_L; i++){
        ch = i + AIN_CH_L;
        ADC1->CSR = 0x00 | ch; // clear EOC and set channel
        
        ADC1->CR1 |= 0x01; // start conversion
        delayus(2);
        ADC1->CR1 |= 0x01; // start conversion
#ifdef USE_IIR
        buf_adc[i] -= buf_adc[i] >> AIN_SMOOTH; // for IIR
#else
        if (0 == count){  // for moving average
            buf_adc[i] = 0;
        }
#endif
        delayus(4);  // or wait for EOC

        t = ADC1->DR.w;
        buf_adc[i] += t;

        // need to act fast without delay or filter
        if((0 == i) && (t < AIN_UVP_TH) && (!low_voltage_lockout)){
            oled_off(); // save poweer for EEPROM programming
            oled_printline(2, "UVP saving  EEPROM   ");
            if(save_eeprom()){
                init_oled();
                oled_printline(3, "ERROR!!    ");
            } else {
                init_oled();
                oled_printline(3, "Finished   ");
            }
            low_voltage_lockout = true;
        }
    }
    count++;
    r = count >> AIN_SMOOTH;
    if(r){
        count = 0;
    }
    return r;
}

void init(){
    CLK->ICKR = 0x01; // enable HSI 
    CLK->ECKR = 0x00; // disable HSE 
    
    CLK->CMSR = 0xe1; // clock source HSI
    CLK->SWR = 0xe1;  // redundancy, HSI clock
    CLK->SWCR = 0x00; // disable auto switch
    CLK->CKDIVR = CLK_CKDIV_HSI_DIV1;  // Set the frequency to 16 MHz
    CLK->PCKENR1 = 0xFF; // Enable peripherals
    CLK->PCKENR2 = 0x8C; // Enable peripherals
        
    BUZZ_PORT->CR1 |= BUZZ_MSK;
    BUZZ_PORT->DDR |= BUZZ_MSK;
    
    // pull up
    PULLUP_PORT->CR1 |= PULLUP_MSK;
    PULLUP_PORT->DDR |= PULLUP_MSK;
    PULLUP_PORT->ODR |= PULLUP_MSK;
   
    CFG_GCR = 0x01; // use SWIM pin as IO
    BTN_PORT->DDR &= ~BTN_PIN_MSK; // input
    BTN_PORT->CR1 |=  BTN_PIN_MSK; // pull up
    BTN_PORT->DDR &= ~BTN_PIN_MSK; // int
}




void wait_for_eop(){
    while(!(FLASH->IAPSR & 0x04)){ // EOP
         delayus(1); // to reset pipeline?
    }
}

uint8_t save_eeprom(){
    FLASH->DUKR = 0xAE;  // magic word
    FLASH->DUKR = 0x56;

    if(FLASH->IAPSR & 0x08){//unlocked
        FLASH->CR1 = 0x00;
        FLASH->CR2 = 0x40;  // WPRG

        memcpy(EEPROM(OFFSET_TICK), &tick_sec, 4);
        wait_for_eop();
        
        memcpy(EEPROM(OFFSET_E),    &e_mj, 4);
        wait_for_eop();
        
        memcpy(EEPROM(OFFSET_CAP),  &cap_mc, 4);
        wait_for_eop();
        
        memcpy(EEPROM(OFFSET_SETTINGS),  &settings[0], 4);
        wait_for_eop();
        
        return 0;
    } else {
        // failed unlocking
        return 1;
    }
}

// load persistance data and setup OLED
void load_eeprom(){
    memcpy(&tick_sec, EEPROM(OFFSET_TICK), 4);
    memcpy(&e_mj,     EEPROM(OFFSET_E),    4);
    memcpy(&cap_mc,   EEPROM(OFFSET_CAP),  4);
    memcpy(&settings[0],EEPROM(OFFSET_SETTINGS), 4);

    oled_write_buf_cmd(settings, 2);
}

// turn OLED 180deg
void toggle_orient(){
    if (0xa0 == settings[0]){
        settings[0] = 0xa1;
        settings[1] = 0xc8;
    } else {
        settings[0] = 0xa0;
        settings[1] = 0xc0;
    }
    oled_clr();
    save_eeprom();
    load_eeprom();
}

//static
void main(void) {
    // should be static, but compiler generates more code for static
    uint8_t result = 0;  // temp result
    uint8_t t_det = 0;   // tick second change detecter
    uint32_t reg_tick;   // save tick second, atomic

    uint32_t clock_h;
    uint8_t clock_m;
    uint8_t clock_s;
    uint8_t t8;

    init();
    init_tick();  // before using delayms
    
    rim();
    
    init_oled();
    oled_clr();
    init_adc();

    load_eeprom();

    oled_printline(0, "V0.1 terrance, 2015");
    oled_printline(2, "4~14V, 0~5A, +/-5%");
    oled_printline(3, "0~1000 Wh, 0-1000Ah");
    oled_printline(1, "built " __DATE__);
    delayms(5000);
    oled_clr();

    while(1){        
        if(poll_adc()){
            // fixed-point
#if (AIN_SMOOTH >= 8)
            volt_mv = ((K_VOLT * (buf_adc[0]>>(AIN_SMOOTH-7))) >> (7 + 8))+B_VOLT;
            amp_ma =  ((K_AMP  * (buf_adc[1]>>(AIN_SMOOTH-7))) >> (7 + 8))+B_AMP;
#else
            volt_mv = ((K_VOLT * buf_adc[0]) >> (AIN_SMOOTH + 8))+B_VOLT;
            amp_ma =  ((K_AMP  * buf_adc[1]) >> (AIN_SMOOTH + 8))+B_AMP;
#endif
            if((volt_mv > 4000) && (low_voltage_lockout)){ // power resume
                low_voltage_lockout = false;
            }

            // smaller than 0 or resolution
            if((amp_ma <= 10) || (amp_ma >= 0x8000)){  
                amp_ma = 0;
            }
            pwr_mw = ((uint32_t)volt_mv) * amp_ma / 1000;
            
            // atomic
            sim();
            reg_tick = tick_sec;
            rim();
            
            // capacity gauge
            if(t_det != (reg_tick & 0xff)){
                t_det = (reg_tick & 0xff);
                e_mj += pwr_mw;
                cap_mc += amp_ma;
            }

            memcpy(buf_line, "000.000 V  000.000A   ", 23);
            sprint_u32_fxp(&buf_line[0],  volt_mv, 7, 3, false);
            sprint_u32_fxp(&buf_line[11], amp_ma,  7, 3, false);
            oled_printline(0, buf_line);
            
            clock_s = tick_sec % 60;
            clock_m = tick_sec / 60 % 60;
            clock_h = tick_sec / 3600;
            
            // ruler, 128/6=21 chars/line
            //                0         1         2
            //                012345678901234567890123456
            memcpy(buf_line, "000.00 W 000000:00:00 ", 23);
            sprint_u32_fxp(&buf_line[0], pwr_mw / 10, 6, 2, false); //W with 3-dig frac
            sprint_u32_fxp(&buf_line[9], clock_h, 6, 0, false); // hour
            buf_line[16] = '0' + clock_m / 10; // minute
            buf_line[17] = '0' + clock_m % 10;
//             t8 = 10;
//             divmod_u8_u8(&clock_m, &t8);
//             buf_line[16] = '0' + clock_m; // second
//             buf_line[17] = '0' + t8;

            buf_line[19] = '0' + clock_s / 10; // second
            buf_line[20] = '0' + clock_s % 10;
//             t8 = 10;
//             divmod_u8_u8(&clock_s, &t8);
//             buf_line[19] = '0' + clock_s; // second
//             buf_line[20] = '0' + t8;
            oled_printline(1, buf_line);`
           
            if(!low_voltage_lockout){
                memcpy(buf_line, "    000.00000 Wh      ", 23);
                sprint_u32_fxp(&buf_line[2], e_mj / 36, 11, 5, false); // mJ to Wh with 5-dig frac
                oled_printline(2, buf_line);
                
                memcpy(buf_line, "    000.00000 Ah      ", 23);
                sprint_u32_fxp(&buf_line[2], cap_mc / 36, 11, 5, false); //mc to Ah with 5-dig frac
                oled_printline(3, buf_line);
            }
        }

        // button, 
        // click to rotate OLED
        // hold to reset counters
        if(btn_stat & BTN_EVENT){
            btn_stat &= ~BTN_EVENT;
            switch(btn_stat){
                case (BTN_HOLD):  // clear and erase
                    tick_sec = 0;
                    tick_2ms = 0;
                    e_mj = 0;
                    cap_mc = 0;
                    toggle_orient(); // turn OLED back. save and reload
                    break;
                case (BTN_DOWN):
                    toggle_orient();
                    break;
                case (BTN_UP):
                    break;
                default:
                    break;
            }
            btn_stat = 0;  // clear it again, buggy compiler
        }
    } // end of while(1)
}

