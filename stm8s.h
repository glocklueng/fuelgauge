/*
    stm8s.h - STM8 registers and support
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

// TODO: incomplete and unfinished

#ifndef _STM8S_H
#define _STM8S_H

#include <stdint.h>

typedef struct{
    uint8_t ODR;
    uint8_t IDR;
    uint8_t DDR;
    uint8_t CR1;
    uint8_t CR2;
} stm8s_gpio_str;

#define PORTA ((volatile stm8s_gpio_str*)0x5000)
#define PORTB ((volatile stm8s_gpio_str*)0x5005)
#define PORTC ((volatile stm8s_gpio_str*)0x500A)
#define PORTD ((volatile stm8s_gpio_str*)0x500F)
#define PORTE ((volatile stm8s_gpio_str*)0x5014)
#define PORTF ((volatile stm8s_gpio_str*)0x5019)

// Flash and EEPROM

#define EEPROM(offset) ((uint8_t*)(0x4000+offset))

typedef struct {
    uint8_t CR1;
    uint8_t CR2;
    uint8_t NCR2;
    uint8_t FPR;
    uint8_t NFPR;
    uint8_t IAPSR;
    uint8_t RESERVED1[2];
    uint8_t PUKR;
    uint8_t RESERVED2;
    uint8_t DUKR;
} stm8s_flash_str;

#define FLASH ((volatile stm8s_flash_str*)0x505A)

/* CLOCK */
typedef struct {
    uint8_t ICKR       ;
    uint8_t ECKR       ;
    uint8_t RESERVE1   ;
    uint8_t CMSR       ;
    uint8_t SWR        ;
    uint8_t SWCR       ;
    uint8_t CKDIVR     ;
    uint8_t PCKENR1    ;
    uint8_t CSSR       ;
    uint8_t CCOR       ;
    uint8_t PCKENR2    ;
    uint8_t RESERVE2   ;
    uint8_t HSITRIMR   ;
    uint8_t SWIMCCR    ; 
} mcu_clk_t;

#define CLK ((volatile mcu_clk_t*)0x50C0)
#define CLK_CKDIV_HSI_DIV1    0x00
#define CLK_CKDIV_HSI_DIV2    0x08
#define CLK_CKDIV_HSI_DIV4    0x10
#define CLK_CKDIV_HSI_DIV8    0x18
#define CLK_CKDIV_CPU_DIV1    0x01
#define CLK_CKDIV_CPU_DIV2    0x01
#define CLK_CKDIV_CPU_DIV4    0x02
#define CLK_CKDIV_CPU_DIV8    0x03
#define CLK_CKDIV_CPU_DIV16   0x04
#define CLK_CKDIV_CPU_DIV32   0x05
#define CLK_CKDIV_CPU_DIV64   0x06
#define CLK_CKDIV_CPU_DIV128  0x07

/*  SPI  */
typedef struct {
    uint8_t CR1;    
    uint8_t CR2;    
    uint8_t ICR;    
    uint8_t SR;     
    uint8_t DR;     
    uint8_t CRCPR;  
    uint8_t RXCRCR; 
    uint8_t TXCRCR; 
} mcu_spi_t;

#define SPI ((volatile mcu_spi_t*)0x5200)
#define SPI_SR_OVR 0x40
#define SPI_SR_BSY 0x80
#define SPI_SR_MODF 0x20
#define SPI_SR_CRCERR 0x10
#define SPI_SR_WKUP 0x08
#define SPI_SR_TXE 0x02
#define SPI_SR_RXNE 0x01




/*  UART  */
typedef struct {
    uint8_t SR;
    uint8_t DR;
    uint8_t BRR1;
    uint8_t BRR2;
    uint8_t CR1;
    uint8_t CR2;
    uint8_t CR3;
    uint8_t CR4;
    uint8_t CR5;
    uint8_t GTR;
    uint8_t PSCR;
} stm8s_uart_str;

#define UART1 ((volatile stm8s_uart_str*)0x5230)

/* USART_CR1 bits */
#define USART_CR1_R8 (1 << 7)
#define USART_CR1_T8 (1 << 6)
#define USART_CR1_UARTD (1 << 5)
#define USART_CR1_M (1 << 4)
#define USART_CR1_WAKE (1 << 3)
#define USART_CR1_PCEN (1 << 2)
#define USART_CR1_PS (1 << 1)
#define USART_CR1_PIEN (1 << 0)

/* USART_CR2 bits */
#define USART_CR2_TIEN (1 << 7)
#define USART_CR2_TCIEN (1 << 6)
#define USART_CR2_RIEN (1 << 5)
#define USART_CR2_ILIEN (1 << 4)
#define USART_CR2_TEN (1 << 3)
#define USART_CR2_REN (1 << 2)
#define USART_CR2_RWU (1 << 1)
#define USART_CR2_SBK (1 << 0)

/* USART_CR3 bits */
#define USART_CR3_LINEN (1 << 6)
#define USART_CR3_STOP2 (1 << 5)
#define USART_CR3_STOP1 (1 << 4)
#define USART_CR3_CLKEN (1 << 3)
#define USART_CR3_CPOL (1 << 2)
#define USART_CR3_CPHA (1 << 1)
#define USART_CR3_LBCL (1 << 0)

/* USART_SR bits */
#define USART_SR_TXE (1 << 7)
#define USART_SR_TC (1 << 6)
#define USART_SR_RXNE (1 << 5)
#define USART_SR_IDLE (1 << 4)
#define USART_SR_OR (1 << 3)
#define USART_SR_NF (1 << 2)
#define USART_SR_FE (1 << 1)
#define USART_SR_PE (1 << 0)

// 16-bit advanced timer
typedef struct {
    uint8_t CR1   ;
    uint8_t CR2   ;
    uint8_t SMCR  ;
    uint8_t ETR   ;
    uint8_t IER   ;
    uint8_t SR1   ;
    uint8_t SR2   ;
    uint8_t EGR   ;
    uint8_t CCMR1 ;
    uint8_t CCMR2 ;
    uint8_t CCMR3 ;
    uint8_t CCMR4 ;
    uint8_t CCER1 ;
    uint8_t CCER2 ;
    uint8_t CNTRH ;
    uint8_t CNTRL ;
    uint8_t PSCRH ;
    uint8_t PSCRL ;
    uint8_t ARRH  ;
    uint8_t ARRL  ;
    uint8_t RCR   ;
    uint8_t CCR1H ;
    uint8_t CCR1L ;
    uint8_t CCR2H ;
    uint8_t CCR2L ;
    uint8_t CCR3H ;
    uint8_t CCR3L ;
    uint8_t CCR4H ;
    uint8_t CCR4L ;
    uint8_t BKR   ;
    uint8_t DTR   ;
    uint8_t OISR  ;
} stm8s_tim1_str;



// 16-bit timer
typedef struct {
    uint8_t CR1   ;
    uint8_t _RESERVE1[2];
    uint8_t IER   ;
    uint8_t SR1   ;
    uint8_t SR2   ;
    uint8_t EGR   ;
    uint8_t CCMR1 ;
    uint8_t CCMR2 ;
    uint8_t CCMR3 ;
    uint8_t CCER1 ;
    uint8_t CCER2 ;
    uint8_t CNTRH ;
    uint8_t CNTRL ;
    uint8_t PSCR  ;
    uint8_t ARRH  ;
    uint8_t ARRL  ;
    uint8_t RCR   ;
    uint8_t CCR1H ;
    uint8_t CCR1L ;
    uint8_t CCR2H ;
    uint8_t CCR2L ;
    uint8_t CCR3H ;
    uint8_t CCR3L ;
} stm8s_tim235_str;

// 8-bit basic timer
typedef struct {
    uint8_t CR1   ;
    uint8_t _RESERVE1[2];
    uint8_t IER   ;
    uint8_t SR    ;
    uint8_t EGR   ;
    uint8_t CNTR  ;
    uint8_t PSCR ;
    uint8_t ARR  ;
    uint8_t RCR   ;
} stm8s_tim46_str;

#define TIM1 ((volatile stm8s_tim1_str*)0x5250)
#define TIM2 ((volatile stm8s_tim235_str*)0x5300)
//#define TIM3 ((volatile stm8s_tim235_str*)0x5320)
#define TIM4 ((volatile stm8s_tim46_str*)0x5340)
//#define TIM5 ((volatile stm8s_tim235_str*)0x5360)
//#define TIM6 ((volatile stm8s_tim46_str*)0x5380)


/* ------------------- TIMERS ------------------- */
/* TIM_IER bits */
#define TIM_IER_BIE (1 << 7)
#define TIM_IER_TIE (1 << 6)
#define TIM_IER_COMIE (1 << 5)
#define TIM_IER_CC4IE (1 << 4)
#define TIM_IER_CC3IE (1 << 3)
#define TIM_IER_CC2IE (1 << 2)
#define TIM_IER_CC1IE (1 << 1)
#define TIM_IER_UIE (1 << 0)

/* TIM_CR1 bits */
#define TIM_CR1_APRE (1 << 7)
#define TIM_CR1_CMSH (1 << 6)
#define TIM_CR1_CMSL (1 << 5)
#define TIM_CR1_DIR (1 << 4)
#define TIM_CR1_OPM (1 << 3)
#define TIM_CR1_URS (1 << 2)
#define TIM_CR1_UDIS (1 << 1)
#define TIM_CR1_CEN (1 << 0)

/* TIM_SR1 bits */
#define TIM_SR1_BIF (1 << 7)
#define TIM_SR1_TIF (1 << 6)
#define TIM_SR1_COMIF (1 << 5)
#define TIM_SR1_CC4IF (1 << 4)
#define TIM_SR1_CC3IF (1 << 3)
#define TIM_SR1_CC2IF (1 << 2)
#define TIM_SR1_CC1IF (1 << 1)
#define TIM_SR1_UIF (1 << 0)

// ADC1
// TODO: union for all 16-bit regs

typedef struct{
    union{
        uint8_t bytes[20];
        uint16_t words[10]; // they are big-endian
    } DBxR;
    uint8_t _RESERVE1[12];
    uint8_t CSR;
    uint8_t CR1;
    uint8_t CR2;
    uint8_t CR3;
    union{
        uint16_t w;
        uint8_t b[2];
    } DR;
    uint8_t TDRH;
    uint8_t TDRL;
    uint8_t HTRH;
    uint8_t HTRL;
    uint8_t LTRH;
    uint8_t LTRL;
    uint8_t AWSRH;
    uint8_t AWSRL;
    uint8_t AWCRH;
    uint8_t AWCRL;
} stm8s_adc1_str;

#define ADC1 ((volatile stm8s_adc1_str*)0x53E0)


#define ADC1_DRH ((uint8_t*)0x5404)
#define ADC1_DRL ((uint8_t*)0x5405)

// SWIM
typedef struct {
    uint8_t CSR;
} stm8s_swim_str;

#define SWIM ((volatile stm8s_swim_str*)0x7F80)
#define CFG_GCR (*(volatile uint8_t*)0x7F60)

#define sim() \
  __asm\
  SIM\
  __endasm

#define rim() \
  __asm\
  RIM\
  __endasm

#define nop() \
  __asm\
  NOP\
  __endasm

#endif // _STM8S_H

