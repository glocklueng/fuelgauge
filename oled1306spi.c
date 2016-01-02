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

// 
// support STM8 hard SPI or GPIO emulated soft SPI
//

#include "common.h"
#include "oled1306spi.h"
#include "font_cp437_5x7.h"
#include "timing.h"


// CS is not connected to MCU and tied low
#define OLED_CS_PINN  3
#define OLED_DC_PINN  5
#define OLED_RST_PIN  4

#define OLED_PORT    PORTC
// #define OLED_OMASK  ((1<<OLED_CS_PINN)|(1<<OLED_DC_PINN))   // or 0xF0?
#define OLED_OMASK  0xF0
#define OLED_CMD  0
#define OLED_DAT  1

#define SOFT_SPI 1
#define SOFT_SPI_PORT PORTC
#define SOFT_SPI_SCK   6
#define SOFT_SPI_MOSI  7


static const uint8_t SSD1306_INIT[] ={
    0xAE, // off
    
    0xd5, // OSC div
    0x80, 
    
    0x81, // contrast
    0x10,

    0xa1, // segment remap

    0xa6, // normal/inv

    0xa8, // multiplex ratio
    0x1f, // 1/32

    0xc8, // scan dir
    
    0xd3, // offset
    0x00, 
    
    0xd9, // precharge
    0x33, 
    
    0xda, // COM pins
    0x00, 
    
    0xdb, // vcomh
    0x40, 
    
    
    0x00, // low col
    0x10, // high col
    0x40, // start line
    0xb0, // page addr
    
    0x20, // addressing mode
    0x02, // 0: H, 1:V, 2: page, 3-invalid
    
    0x8d, // charge pump
    0x14, 
    0xaf  // on
};

void spi_write(uint8_t d){
#ifdef SOFT_SPI
    uint8_t i;
    uint8_t tp00; // port ODR cache
    uint8_t tp10; 
    uint8_t tp01; 
    uint8_t tp11; 
    tp00 = OLED_PORT->ODR & ~((1<<SOFT_SPI_MOSI)|(1<<SOFT_SPI_SCK)); //data 0, clk low
    tp10 = tp00 | (1<<SOFT_SPI_MOSI); // data 1, clk low
    tp01 = tp00 | (1<<SOFT_SPI_SCK);  // data 0, clk low
    tp11 = tp01 | (1<<SOFT_SPI_MOSI); // data 1, clk high

    for(i = 0x80; i != 0; i >>=1){
        if(i & d){
            OLED_PORT->ODR = tp10;
            OLED_PORT->ODR = tp11;
            OLED_PORT->ODR = tp10;
        } else {
            OLED_PORT->ODR = tp00;
            OLED_PORT->ODR = tp01;
            OLED_PORT->ODR = tp00;
        }
    }
#else
    while(0 == (SPI->SR & SPI_SR_TXE)){
        ; // wait
    }
    SPI.DR = d;
#endif
}

void oled_spi_begin(uint8_t cd){
    OLED_PORT->ODR &= ~(1<<OLED_CS_PINN);
    if(cd){
        OLED_PORT->ODR |= (1<<OLED_DC_PINN);
    } else {
        OLED_PORT->ODR &= ~(1<<OLED_DC_PINN);
    }
}

void oled_spi_end(){
    OLED_PORT->ODR |= (1<<OLED_CS_PINN);
}



void init_spi(){
#ifdef SOFT_SPI
    SOFT_SPI_PORT->DDR |= (1<<SOFT_SPI_SCK) | (1<<SOFT_SPI_MOSI);
    SOFT_SPI_PORT->CR1 |= (1<<SOFT_SPI_SCK) | (1<<SOFT_SPI_MOSI);
    SOFT_SPI_PORT->ODR &= ~((1<<SOFT_SPI_SCK) | (1<<SOFT_SPI_MOSI));
#else
    SPI->CR1 = 0x44; //SPE=1,Baud=fMaster/2, Master
    SPI->CR2 = 0xC0; // soft NSS
#endif
}

void init_oled(){
    uint8_t i;
    OLED_PORT->DDR |= OLED_OMASK;
    OLED_PORT->CR1 |= OLED_OMASK;
    init_spi();

    delayus(300);

    OLED_PORT->ODR &= ~(1<<OLED_RST_PIN);
    delayus(30);
    OLED_PORT->ODR |= (1<<OLED_RST_PIN);

    oled_spi_begin(OLED_CMD);
    for (i = 0; i < sizeof(SSD1306_INIT); i++){
        spi_write(SSD1306_INIT[i]);
    }
    
    oled_spi_end();
}

int8_t lfsr16_8(uint16_t iv){
    static uint16_t sr;
    uint8_t i;
    if(0 != iv){
        sr = iv;
    }
    for(i = 0; i < 5; i++){
        if((0 == (0x01 & sr)) || 0 == sr){
            //lfsr = (lfsr >> 1) ^ 0xa8;
            sr = (sr >> 1) ^ 0xb400;
        } else {
            sr >>= 1;
        }
    }
    return sr & 0xff;
}

void oled_print(const char *s){
    uint8_t i, k;
    oled_spi_begin(OLED_DAT);
    for(i = 0; i < 26 && 0 != s[i] ; i++){
        for(k = 0; k < 5; k++){
            spi_write(font_cp437_5x7[s[i]][k]);
        }
        spi_write(0);
    }
    oled_spi_end();
}


void oled_write_buf_dat(const char *s, size_t n){
    size_t i;
    oled_spi_begin(OLED_DAT);
    for(i = 0; i < n; i++){
        spi_write(s[i]);
    }
    oled_spi_end();
}

void oled_write_buf_cmd(const char *s, size_t n){
    size_t i;
    oled_spi_begin(OLED_CMD);
    for(i = 0; i < n; i++){
        spi_write(s[i]);
    }
    oled_spi_end();
}

void oled_off(){
    oled_spi_begin(OLED_CMD);
    spi_write(0xAE);
    oled_spi_end();
}

void oled_on(){
    oled_spi_begin(OLED_CMD);
    spi_write(0xAF);
    oled_spi_end();
}

void oled_printline(uint8_t line, const char *s){
    oled_spi_begin(OLED_CMD);
    spi_write(0xB0 | line);
    spi_write(0x21);
    spi_write(0x00); // first column
    spi_write(0x7f);
    
    oled_print(s);
}

void test_oled_noise(uint16_t iv){
    uint8_t i;
    uint8_t j;

    lfsr16_8(iv);
    
    oled_spi_begin(OLED_CMD);
    i = 0;
    spi_write(0x00);
    spi_write(0x10);
    spi_write(0x40);
    oled_spi_begin(OLED_DAT);
    for(j = 0; j < 4; j++){
        for(i = 0; i < 128; i++){
            spi_write(lfsr16_8(0));
        }
    }
    oled_spi_end();
}


void oled_fill(uint8_t c){
    uint8_t i;
    uint8_t j;

    oled_spi_begin(OLED_CMD);
    spi_write(0x00);
    spi_write(0x10);
    spi_write(0x40);
    for(j = 0; j < 4; j++){
        oled_spi_begin(OLED_CMD);
        spi_write(0xB0 | j);
        spi_write(0x21);
        spi_write(0x00);
        spi_write(0x7f);
        oled_spi_begin(OLED_DAT);
        for(i = 0; i < 128; i++){
            spi_write(c);
        }
    }
    oled_spi_end();
}

void oled_clr(){
    oled_fill(0);
}

void test_oled_text(uint16_t iv){
    uint8_t i;
    uint8_t j;
    uint8_t k;

    uint8_t c;

    lfsr16_8(iv);
    
    oled_spi_begin(OLED_CMD);
    i = 0;
    spi_write(0x00);
    spi_write(0x10);
    spi_write(0x40);
    for(j = 0; j < 4; j++){
        oled_spi_begin(OLED_CMD);
        spi_write(0xB0 | j);
        oled_spi_begin(OLED_DAT);
        for(i = 0; i < 25; i++){
            c = lfsr16_8(0);
            for(k = 0; k < 5; k++){
                spi_write(font_cp437_5x7[c][k]);
            }
            spi_write(0);
        }
    }
    oled_spi_end();
}


