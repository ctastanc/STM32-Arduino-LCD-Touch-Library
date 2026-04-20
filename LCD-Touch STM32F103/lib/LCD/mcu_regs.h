//#pragma once
#ifndef _MCU_REGS_H_
#define _MCU_REGS_H_

#include "lcd_regs.h"

#define MCU_8_BIT_BUS  1
#define CPU_F 84000000
// ILI9341 Touch pins are very important. If XM and YP are not analog pins, it will not work. 
// These correspond to XM:RS and YP:CS pins. XP and YM correspond to Dataport PA0 and PA1.
#define XM PB0 // ADC8 must be an analog pin, use "An" notation!
#define YP PB1 // ADC9 must be an analog pin, use "An" notation!
#define XP PA0 // can be a digital pin
#define YM PA1 // can be a digital pin
// Lcd pins
#define RS_PIN  GPIO_PIN_0  // PB0
#define CS_PIN  GPIO_PIN_1  // PB1
#define RD_PIN  GPIO_PIN_14 // PB14
#define RST_PIN GPIO_PIN_15 // PB15
//The WR pin is on the data_port, and the WR_L state is triggered by the BSRR register.
#define WR_PIN  GPIO_PIN_8  // PA8

#define RS_PORT  GPIOB
#define CS_PORT  GPIOB
#define RD_PORT  GPIOB
#define RST_PORT GPIOB
#define WR_PORT  GPIOA

#define RD_L    {RD_PORT->BSRR  = RD_PIN << 16;} // RD_L LOW
#define RD_H    {RD_PORT->BSRR  = RD_PIN;}       // RD_H HIGH
#define WR_L    {WR_PORT->BSRR  = WR_PIN << 16;} 
#define WR_H    {WR_PORT->BSRR  = WR_PIN;} 
#define RS_CMD  {RS_PORT->BSRR  = RS_PIN << 16;}
#define RS_DATA {RS_PORT->BSRR  = RS_PIN;}
#define CS_L    {CS_PORT->BSRR  = CS_PIN << 16;}
#define CS_H    {CS_PORT->BSRR  = CS_PIN;}
#define RST_L   {RST_PORT->BSRR = RST_PIN << 16;}
#define RST_H   {RST_PORT->BSRR = RST_PIN;}
//                               LCD  0   1   2   3   4   5   6   7
#define DATA_PORT GPIOA // DATA_PORT PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7
#define DATA_MASK 0x01FF0000UL

#if (CPU_F > 168000000 && CPU_F <= 180000000) // 5.5ns
    #define RD_DELAY { __asm__ volatile(".rept 8 \n\t nop \n\t .endr"); }
    #define {WRL_DELAY WR_L;WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (CPU_F > 144000000 && CPU_F <= 168000000) // 5.9ns
    #define RD_DELAY { __asm__ volatile(".rept 7 \n\t nop \n\t .endr"); }
    #define {WRL_DELAY WR_L;WR_L;}//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (CPU_F > 120000000 && CPU_F <= 144000000)  // 6.9ns
    #define RD_DELAY { __asm__ volatile(".rept 6 \n\t nop \n\t .endr"); }
    #define WRL_DELAY WR_L//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (CPU_F > 100000000 && CPU_F <= 120000000) // 7.6ns
    #define RD_DELAY { __asm__ volatile(".rept 5 \n\t nop \n\t .endr"); }
    #define WRL_DELAY WR_L//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (CPU_F > 84000000 && CPU_F <= 100000000) // 10ns
    #define RD_DELAY { __asm__ volatile(".rept 4 \n\t nop \n\t .endr"); }
    #define WRL_DELAY WR_L//__asm__ volatile("nop");
    #define WRH_DELAY 
#elif (CPU_F <= 84000000L) // 11.9ns
    #define RD_DELAY { __asm__ volatile(".rept 3 \n\t nop \n\t .endr"); }
    #define WRL_DELAY 
    #define WRH_DELAY 
#endif

#if(MCU_8_BIT_BUS==1) //8 bit bus
    #define ENABLE_AND_SET_PORTS() { \
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;\
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;\
    DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFC0000) | 0x00015555; /*PA0..PA8 Output*/\
    DATA_PORT->OTYPER &= ~0x01FF; /*PA0..PA8 Push-Pull*/\
    DATA_PORT->OSPEEDR |= 0x0003FFFF; /*PA0..PA8 Very High Speed m:0x00015555 h:0x0002AAAA vh:0x0003FFFF*/\
    GPIOB->MODER = (GPIOB->MODER & 0xFFFFFF0) | 0x50000005; /*PB0-1-14-15 Output*/\
    GPIOB->OTYPER &= ~0xC003; /*PB0-1-14-15 Push-Pull*/\
    GPIOB->OSPEEDR |= 0xF000000F; /*PB0-1-14-15 Very High Speed m:0x0005005 h:0x000A00A vh:0x000F00F*/}
    #define SET_WRITE_DIR() DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFF0000) | 0x00005555; /*PA0..PA7 Output*/
    #define SET_READ_DIR()  DATA_PORT->MODER &= ~0x0000FFFF; /*PA0..PA7 Input*/

    //#define WRITE8(d) { DATA_PORT->BSRR = DATA_MASK | (d); WR_L; WR_H; } //For frequencies above 84Mhz.
    #define WRITE8(d) { DATA_PORT->BSRR = DATA_MASK | (d); WRL_DELAY; WR_H; WRH_DELAY; } //WR_L state is triggered by the BSRR register.
    #define READ8(dst) { RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT->IDR & ~(0xFF00U)); RD_H; }
    #define READ16(dst) { uint8_t hi; READ8(hi); READ8(dst); dst |= (hi << 8); }
    #define CMD8(x) { RS_CMD; WRITE8(x); RS_DATA; }
    #define DATA8(x) {WRITE8(x)}
    #define CMD16(x) { RS_CMD; WRITE8(x>>8); WRITE8(x&0XFF); RS_DATA; }
    #define DATA16(x) { WRITE8(x>>8); WRITE8(x&0xFF); }
    #define CMDDATA8(a,d) { RS_CMD; WRITE8(a); RS_DATA; WRITE8(d); }
    #define CMDDATA16(a,d) { RS_CMD; WRITE8(a>>8); WRITE8(a&0XFF); RS_DATA; WRITE8(d>>8); WRITE8(d&0XFF); }
#endif

#if(MCU_8_BIT_BUS==0) //16 bit bus
    //                                LCD   8   9  10  11  12  13  14  15
    #define DATA_PORT2 GPIOB // DATA_PORT2 PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10
    #define DATA_MASK2 0xFF000000UL

    #define ENABLE_AND_SET_PORTS() { \
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;\
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;\
    DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFC0000) | 0x00015555; /*PA0..PA8 Output*/\
    DATA_PORT->OTYPER &= ~0x01FF; /*PA0..PA8 Push-Pull*/\
    DATA_PORT->OSPEEDR |= 0x0003FFFF; /*PA0..PA8 Very High Speed m:0x00015555 h:0x0002AAAA vh:0x0003FFFF*/\
    DATA_PORT2->MODER = (DATA_PORT2->MODER & 0xFFC0003F) | 0x00155540; /*PB3..PB10 Output*/\
    DATA_PORT2->OTYPER &= ~0x07F8; /*PB3..PB10 Push-Pull*/\
    DATA_PORT2->OSPEEDR |= 0x003FFFC0; /*PB3..PB10 Very High Speed*/\
    GPIOB->MODER = (GPIOB->MODER & 0xFFFFFF0) | 0x50000005; /*PB0-1-14-15 Output*/\
    GPIOB->OTYPER &= ~0xC003; /*PB0-1-14-15 Push-Pull*/\
    GPIOB->OSPEEDR |= 0xF000000F; /*PB0-1-14-15 Very High Speed*/}
    // PA0..PA8 and PB3..PB10 Output  
    #define SET_WRITE_DIR() {DATA_PORT->MODER = (DATA_PORT->MODER & 0xFFFF0000) | 0x00005555;\
                         DATA_PORT2->MODER = (DATA_PORT2->MODER & 0x3FC0003F) | 0x00155540;} 
    // PA0..PA8 and PB3..PB10 Input 
    #define SET_READ_DIR()  {DATA_PORT->MODER &= ~0x0000FFFF; /*PA0..PA7 Input*/\
                            DATA_PORT2->MODER &= ~0x003FFFC0; /*PB3..PB10 Input*/}
     // WR_L state is triggered by the BSRR register.    
    #define WRITE16(d) {DATA_PORT->BSRR = DATA_MASK | (d&0xFF);\
                       DATA_PORT2->BSRR = DATA_MASK2 | ((d&0xFF00)>>5); WRL_DELAY; WR_H; WRH_DELAY;}
    #define WRITE8(d) { DATA_PORT->BSRR = DATA_MASK | (d); WRL_DELAY; WR_H; WRH_DELAY;} //WR_L state is triggered by the BSRR register.
    #define READ8(dst) { RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT->IDR & ~(0xFF00U)); RD_H; }
    #define READ16(dst) { uint16_t hi; RD_L; RD_DELAY; dst = (uint8_t)(DATA_PORT->IDR & ~(0xFF00U)); \
                        hi = (uint16_t)(DATA_PORT2->IDR & ~(0xF807U)); dst |= ((hi << 5)&0xFF00); RD_H;}
    #define CMD8(x) { RS_CMD; WRITE8(x); RS_DATA; }
    #define DATA8(x) {WRITE8(x)}
    #define CMD16(x) { RS_CMD; WRITE16(x); RS_DATA; }
    #define DATA16(x) { WRITE16(x); }
    #define CMDDATA8(a,d) { RS_CMD; WRITE8(a); RS_DATA; WRITE8(d); }
    #define CMDDATA16(a,d) { RS_CMD; WRITE16(a); RS_DATA; WRITE16(d); }
#endif

#define SET_X(x1,x2) {CMD8(XS); DATA16(x1); DATA16(x2);}
#define SET_Y(y1,y2) {CMD8(YS); DATA16(y1); DATA16(y2);}
#define BLOCK4(c) {DATA16(c); DATA16(c); DATA16(c); DATA16(c);}
#define BLOCK8(c) {BLOCK4(c); BLOCK4(c);} 
   
#if (LCD_DRIVER == ID_932X)
    #define SET_ADDR_WINDOW(x1, y1, x2, y2) {\
    int16_t _nx1 = (x1), _ny1 = (y1), _nx2 = (x2), _ny2 = (y2); \
    int16_t _x, _y, _t; \
    switch(this->rotation) { \
        case 1: \
            _t=_ny1; _ny1=_nx1; _nx1=WIDTH-1-_ny2; _ny2=_nx2; _nx2=WIDTH-1-_t; \
            _x=_nx2; _y=_ny1; break; \
        case 2: \
            _t=_nx1; _nx1=WIDTH-1-_nx2; _nx2=WIDTH-1-_t; \
            _t=_ny1; _ny1=HEIGHT-1-_ny2; _ny2=HEIGHT-1-_t; \
            _x=_nx2; _y=_ny2; break; \
        case 3: \
            _t=_nx1; _nx1=_ny1; _ny1=HEIGHT-1-_nx2; _nx2=_ny2; _ny2=HEIGHT-1-_t; \
            _x=_nx1; _y=_ny2; break; \
        default: _x = _nx1; _y = _ny1; break; \
    } \
    CMDDATA16(ILI932X_HOR_START_AD, _nx1); \
    CMDDATA16(ILI932X_HOR_END_AD,   _nx2); \
    CMDDATA16(ILI932X_VER_START_AD, _ny1); \
    CMDDATA16(ILI932X_VER_END_AD,   _ny2); \
    CMDDATA16(ILI932X_GRAM_HOR_AD,  _x); \
    CMDDATA16(ILI932X_GRAM_VER_AD,  _y); CMD8(ILI932X_START_OSC);}
#elif (LCD_DRIVER == ID_7575)
    #define SET_ADDR_WINDOW(x1, y1, x2, y2) {\
    CMDDATA8(HX8347G_COLADDRSTART_HI, (x1) >> 8); \
    CMDDATA8(HX8347G_COLADDRSTART_LO, (x1)); \
    CMDDATA8(HX8347G_ROWADDRSTART_HI, (y1) >> 8); \
    CMDDATA8(HX8347G_ROWADDRSTART_LO, (y1)); \
    CMDDATA8(HX8347G_COLADDREND_HI,   (x2) >> 8); \
    CMDDATA8(HX8347G_COLADDREND_LO,   (x2)); \
    CMDDATA8(HX8347G_ROWADDREND_HI,   (y2) >> 8); \
    CMDDATA8(HX8347G_ROWADDREND_LO,   (y2)); } 
#else
    #define SET_ADDR_WINDOW(x1, y1, x2, y2) {\
    CMD8(XS); DATA16(x1); DATA16(x2); \
    CMD8(YS); DATA16(y1); DATA16(y2); }
#endif

/*#define DRAW_PIXEL(x, y, color) \
if (!((uint32_t)(x) > width || (uint32_t)(y) > height)) { \
    CS_ACTIVE; \
    SET_ADDR_WINDOW(x, y, x, y); \
    CMD8(0x2C); \
    DATA16(color); \
    CS_IDLE; \
}*/

const inline char* Get_STM32_Name(void) {
    uint32_t dev_id = HAL_GetDEVID(); 
    switch (dev_id) {
        // STM32 F Serisi
        case 0x412: return "STM32F1 Low-density"; //72mhz
        case 0x410: return "STM32F1 Medium-density"; //72mhz
        case 0x414: return "STM32F1 High-density"; //72mhz
        case 0x430: return "STM32F1 XL-density"; 
        case 0x411: return "STM32F2 Series";
        case 0x440: return "STM32F030x4/x6/x8 / F05x"; //48mhz
        case 0x444: return "STM32F030xC"; //48mhz
        case 0x445: return "STM32F042 / F070x6";
        case 0x448: return "STM32F071 / F072 / F091";
        case 0x413: return "STM32F405/407/415/417"; //168mhz
        case 0x419: return "STM32F42x/43x"; //180mhz
        case 0x423: return "STM32F401xB/C"; //84mhz
        case 0x433: return "STM32F401xD/E";
        case 0x431: return "STM32F411xC/E"; //100mhz
        case 0x449: return "STM32F74x/75x";
        case 0x451: return "STM32F76x/77x";
        // STM32 L Serisi (Düşük Güç)
        case 0x416: return "STM32L1 Medium-density / Plus";
        case 0x427: return "STM32L1 High-density";
        case 0x417: return "STM32L0 Series";
        case 0x435: return "STM32L4 Series";
        // STM32 G Serisi (Yeni Nesil)
        case 0x460: return "STM32G0 Series";
        case 0x466: return "STM32G4 Series";
        // STM32 H Serisi (Yüksek Performans)
        case 0x450: return "STM32H74x/75x";
        case 0x480: return "STM32H72x/73x";
        // Bilinmeyen
        default: return "Unknown or new STM32";
    }
}

//const char* Get_STM32_Name(void);

#endif // mcu_regs