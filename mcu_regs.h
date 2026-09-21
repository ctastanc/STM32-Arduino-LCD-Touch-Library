#pragma once

#include <lcd_regs.h>
#include <stdint.h>

#if defined(STM32F0xx)
  #include "stm32f0xx_ll_bus.h"
  #include "stm32f0xx_ll_gpio.h"
#elif defined(STM32F1xx)
  #include "stm32f1xx_ll_bus.h"
  #include "stm32f1xx_ll_gpio.h"
#elif defined(STM32F2xx)
  #include "stm32f2xx_ll_bus.h"
  #include "stm32f2xx_ll_gpio.h"
#elif defined(STM32F3xx)
  #include "stm32f3xx_ll_bus.h"
  #include "stm32f3xx_ll_gpio.h"
#elif defined(STM32F4xx)
  #include "stm32f4xx_ll_bus.h"
  #include "stm32f4xx_ll_gpio.h"
#elif defined(STM32F7xx)
  #include "stm32f7xx_ll_bus.h"
  #include "stm32f7xx_ll_gpio.h"
#elif defined(STM32H7xx)
  #include "stm32h7xx_ll_bus.h"
  #include "stm32h7xx_ll_gpio.h"
#elif defined(STM32G0xx)
  #include "stm32g0xx_ll_bus.h"
  #include "stm32g0xx_ll_gpio.h"
#elif defined(STM32G4xx)
  #include "stm32g4xx_ll_bus.h"
  #include "stm32g4xx_ll_gpio.h"
#elif defined(STM32L0xx)
  #include "stm32l0xx_ll_bus.h"
  #include "stm32l0xx_ll_gpio.h"
#elif defined(STM32L1xx)
  #include "stm32l1xx_ll_bus.h"
  #include "stm32l1xx_ll_gpio.h"
#elif defined(STM32L4xx)
  #include "stm32l4xx_ll_bus.h"
  #include "stm32l4xx_ll_gpio.h"
#elif defined(STM32L5xx)
  #include "stm32l5xx_ll_bus.h"
  #include "stm32l5xx_ll_gpio.h"
#elif defined(STM32WBxx)
  #include "stm32wbxx_ll_bus.h"
  #include "stm32wbxx_ll_gpio.h"
#elif defined(STM32WLxx)
  #include "stm32wlxx_ll_bus.h"
  #include "stm32wlxx_ll_gpio.h"
#elif defined(STM32C0xx)
  #include "stm32c0xx_ll_bus.h"
  #include "stm32c0xx_ll_gpio.h"
#elif defined(STM32U0xx)
  #include "stm32u0xx_ll_bus.h"
  #include "stm32u0xx_ll_gpio.h"
#elif defined(STM32U5xx)
  #include "stm32u5xx_ll_bus.h"
  #include "stm32u5xx_ll_gpio.h"
#elif defined(STM32MP1xx)
  #include "stm32mp1xx_ll_bus.h"
  #include "stm32mp1xx_ll_gpio.h"
#else
  #error "The current microcontroller is an unknown STM32 series or is missing from the library list!"
#endif

static constexpr uint32_t Ctrl_Pins[]  =      {RS, CS, RD, RST};
static constexpr uint32_t Data_Pins_8Bit[]  = {D0, D1, D2, D3, D4, D5, D6, D7, WR};
static constexpr uint32_t Data_Pins_16Bit[] = {D8, D9, D10, D11, D12, D13, D14, D15};
#define DATA_MASK1 0x01FF0000UL
#define DATA_MASK2 0x07F80000UL

#define WR_L    { LL_GPIO_ResetOutputPin(DATA_PORT1, Data_Pins_8Bit[8]); }
#define WR_H    { LL_GPIO_SetOutputPin(DATA_PORT1, Data_Pins_8Bit[8]); }
#define RS_CMD  { LL_GPIO_ResetOutputPin(CTRL_PORT, Ctrl_Pins[0]); }
#define RS_DATA { LL_GPIO_SetOutputPin(CTRL_PORT, Ctrl_Pins[0]); }
#define CS_L    { LL_GPIO_ResetOutputPin(CTRL_PORT, Ctrl_Pins[1]); }
#define CS_H    { LL_GPIO_SetOutputPin(CTRL_PORT, Ctrl_Pins[1]); }
#define RD_L    { LL_GPIO_ResetOutputPin(CTRL_PORT, Ctrl_Pins[2]); }
#define RD_H    { LL_GPIO_SetOutputPin(CTRL_PORT, Ctrl_Pins[2]); }
#define RST_L   { LL_GPIO_ResetOutputPin(CTRL_PORT, Ctrl_Pins[3]); }
#define RST_H   { LL_GPIO_SetOutputPin(CTRL_PORT, Ctrl_Pins[3]); }

#if (TARGET_CPU_FREQ > 168000000 ) // 5.5ns
    #define RD_DELAY { __asm__ volatile(".rept 8 \n\t nop \n\t .endr"); }
    #define WRL_DELAY {WR_L;WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY WR_H;
#elif (TARGET_CPU_FREQ > 144000000 ) // 5.9ns
    #define RD_DELAY { __asm__ volatile(".rept 7 \n\t nop \n\t .endr"); }
    #define WRL_DELAY {WR_L;WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY
#elif (TARGET_CPU_FREQ > 120000000 )  // 6.9ns
    #define RD_DELAY { __asm__ volatile(".rept 6 \n\t nop \n\t .endr"); }
    #define WRL_DELAY {WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (TARGET_CPU_FREQ > 100000000 )  // 7.6ns
    #define RD_DELAY { __asm__ volatile(".rept 5 \n\t nop \n\t .endr"); }
    #define WRL_DELAY {WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (TARGET_CPU_FREQ > 84000000 ) // 10ns
    #define RD_DELAY { __asm__ volatile(".rept 4 \n\t nop \n\t .endr"); }
    #define WRL_DELAY {WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (TARGET_CPU_FREQ <= 84000000L) // 11.9ns
    #define RD_DELAY { __asm__ volatile(".rept 3 \n\t nop \n\t .endr"); }
    #define WRL_DELAY 
    #define WRH_DELAY 
#endif

static inline void enable_gpio_clock(GPIO_TypeDef* GPIOx) {
    #if defined(STM32F1xx) || defined(STM32F3xx)
        if      (GPIOx == GPIOA) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
        else if (GPIOx == GPIOB) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
        else if (GPIOx == GPIOC) LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    #elif defined(STM32F2xx) || defined(STM32F4xx) || defined(STM32F7xx)
        if      (GPIOx == GPIOA) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        else if (GPIOx == GPIOB) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
        else if (GPIOx == GPIOC) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    #elif defined(STM32L4xx) || defined(STM32L5xx) || defined(STM32G4xx) || defined(STM32H7xx) || defined(STM32WBxx) || defined(STM32U5xx)
        if      (GPIOx == GPIOA) LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
        else if (GPIOx == GPIOB) LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
        else if (GPIOx == GPIOC) LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    #elif defined(STM32L0xx) || defined(STM32G0xx) || defined(STM32C0xx) || defined(STM32U0xx) || defined(STM32F0xx)
        if      (GPIOx == GPIOA) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
        else if (GPIOx == GPIOB) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        else if (GPIOx == GPIOC) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    #endif
}

#define LL_SET_PINS(SET, PORT, PINS, NUM, MODE) { for(int i=0; i<NUM;i++) { SET(PORT, PINS[i], MODE); } }

#ifndef STM32F1xx
    #define LL_GPIO_MODE_OUT LL_GPIO_MODE_OUTPUT
    #define DISABLE_JTAG()
    #define LL_PULL8() {\
        LL_SET_PINS(LL_GPIO_SetPinPull, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_PULL_NO); \
        LL_SET_PINS(LL_GPIO_SetPinPull, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_PULL_NO); }
    #if(LCD_SYS_INTERFACE==16)
        #define LL_PULL16() {\
        LL_SET_PINS(LL_GPIO_SetPinPull, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_PULL_NO); }
    #endif
#else
    #define LL_GPIO_MODE_OUT LL_GPIO_MODE_OUTPUT_50MHz
    #define LL_PULL8()
    #define LL_PULL16()
    // Disabling JTAG for F1 (PB3 PB4)
    #define DISABLE_JTAG() { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO); LL_GPIO_AF_Remap_SWJ_NOJTAG(); }
#endif

#if(LCD_SYS_INTERFACE==8)
    #define SET_PORTS() { \
        enable_gpio_clock(DATA_PORT1); \
        enable_gpio_clock(CTRL_PORT); \
        LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_MODE_OUT); \
        LL_SET_PINS(LL_GPIO_SetPinOutputType, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_SET_PINS(LL_GPIO_SetPinSpeed, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_SPEED_FREQ_HIGH); \
        LL_SET_PINS(LL_GPIO_SetPinMode, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_MODE_OUT); \
        LL_SET_PINS(LL_GPIO_SetPinOutputType, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_SET_PINS(LL_GPIO_SetPinSpeed, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_SPEED_FREQ_HIGH); \
        LL_PULL8(); \
    }

    #define SET_WRITE_DIR() LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 8, LL_GPIO_MODE_OUT)
    #define SET_READ_DIR()  LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 8, LL_GPIO_MODE_INPUT)
  
    #define WRITE8(d) { DATA_PORT1->BSRR = DATA_MASK1 | (d); WRL_DELAY; WR_H; WRH_DELAY; }
    #define READ8(dst) { RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT1->IDR & (0x00FFU)); RD_H; }
    #define READ16(dst) { uint8_t hi; READ8(hi); READ8(dst); dst |= (hi << 8); }
    #define CMD8(x) { RS_CMD; WRITE8(x); RS_DATA; }
    #define DATA8(x) {WRITE8(x);}
    #define CMD16(x) { RS_CMD; WRITE8(x>>8); WRITE8(x&0XFF); RS_DATA; }
    #define DATA16(x) { WRITE8(x>>8); WRITE8(x&0xFF); }
    #define CMDDATA8(a,d) { RS_CMD; WRITE8(a); RS_DATA; WRITE8(d); }
    #define CMDDATA16(a,d) { RS_CMD; WRITE8(a>>8); WRITE8(a&0XFF); RS_DATA; WRITE8(d>>8); WRITE8(d&0XFF); }
#endif

#if(LCD_SYS_INTERFACE==16) 
    #define SET_PORTS() { \
        enable_gpio_clock(DATA_PORT1); \
        enable_gpio_clock(DATA_PORT2); \
        enable_gpio_clock(CTRL_PORT); \
        DISABLE_JTAG(); \
        LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_MODE_OUT); \
        LL_SET_PINS(LL_GPIO_SetPinOutputType, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_SET_PINS(LL_GPIO_SetPinSpeed, DATA_PORT1, Data_Pins_8Bit, 9, LL_GPIO_SPEED_FREQ_HIGH); \
        LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_MODE_OUT); \
        LL_SET_PINS(LL_GPIO_SetPinOutputType, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_SET_PINS(LL_GPIO_SetPinSpeed, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_SPEED_FREQ_HIGH); \
        LL_SET_PINS(LL_GPIO_SetPinMode, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_MODE_OUT); \
        LL_SET_PINS(LL_GPIO_SetPinOutputType, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_OUTPUT_PUSHPULL); \
        LL_SET_PINS(LL_GPIO_SetPinSpeed, CTRL_PORT, Ctrl_Pins, 4, LL_GPIO_SPEED_FREQ_HIGH); \
        LL_PULL8(); \
        LL_PULL16(); \
    }
    #define SET_WRITE_DIR() { LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 8, LL_GPIO_MODE_OUT);\
                                LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_MODE_OUT); }
    #define SET_READ_DIR()  { LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT1, Data_Pins_8Bit, 8, LL_GPIO_MODE_INPUT);\
                                LL_SET_PINS(LL_GPIO_SetPinMode, DATA_PORT2, Data_Pins_16Bit, 8, LL_GPIO_MODE_INPUT); }
        
    #define WRITE16(d) {DATA_PORT1->BSRR = DATA_MASK1 | (d&0xFF);\
                       DATA_PORT2->BSRR = DATA_MASK2 | ((d&0xFF00)>>5); WRL_DELAY; WR_H; WRH_DELAY;}
    #define WRITE8(d) { DATA_PORT1->BSRR = DATA_MASK1 | (d); WRL_DELAY; WR_H; WRH_DELAY;} 
    #define READ8(dst) { RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT1->IDR & (0x00FFU)); RD_H; }
    #define READ16(dst) { uint16_t hi; RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT1->IDR & (0x00FFU)); \
                        hi = (uint16_t)(DATA_PORT2->IDR & (0x07F8U)); dst |= ((hi << 5)&0xFF00); RD_H;}
    #define CMD8(x) { RS_CMD; WRITE8(x); RS_DATA; }
    #define DATA8(x) {WRITE8(x);}
    #define CMD16(x) { RS_CMD; WRITE16(x); RS_DATA; }
    #define DATA16(x) { WRITE16(x); }
    #define CMDDATA8(a,d) { RS_CMD; WRITE8(a); RS_DATA; WRITE8(d); }
    #define CMDDATA16(a,d) { RS_CMD; WRITE16(a); RS_DATA; WRITE16(d); }
#endif

#define SET_X(x1,x2) {CMD8(XS); DATA16(x1); DATA16(x2);}
#define SET_Y(y1,y2) {CMD8(YS); DATA16(y1); DATA16(y2);}
#define BLOCK4(c) {DATA16(c); DATA16(c); DATA16(c); DATA16(c);}
#define BLOCK8(c) {BLOCK4(c); BLOCK4(c);}

static inline void set_pin_output(GPIO_TypeDef* port, uint32_t pin) {
    if (!port) return;
    enable_gpio_clock(port);
#if defined(LL_GPIO_MODE_OUTPUT_50MHz) // STM32F1
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT_50MHz);
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
#else // STM32F4, F7, G0, G4, H7, L4 vb.
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(port, pin, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO);
#endif
}

static inline void set_pin_analog(GPIO_TypeDef* port, uint32_t pin) {
    if (!port) return;
    enable_gpio_clock(port);
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_ANALOG);
#if !defined(LL_GPIO_MODE_OUTPUT_50MHz)
    LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO);
#endif
}

static inline void digital_write(GPIO_TypeDef* port, uint32_t pin, uint8_t val) {
    if (!port) return;
    if (val) {
        LL_GPIO_SetOutputPin(port, pin);
    } else {
        LL_GPIO_ResetOutputPin(port, pin);
    }
}