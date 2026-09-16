#pragma once

// Set LCD MODEL
//#define LCD_MODEL ILI9325 //0 0x9325,240,320, LCD_DRIVER ID_932X
//#define LCD_MODEL ILI9328 //1 0x9328,240,320, LCD_DRIVER ID_932X
#define LCD_MODEL ILI9341 //2 0x9341,240,320, LCD_DRIVER ID_9341
//#define LCD_MODEL HX8357D //3 0x9090,320,480, LCD_DRIVER ID_HX8357D
//#define LCD_MODEL HX8347G //4 0x7575,240,320, LCD_DRIVER ID_7575
//#define LCD_MODEL HX8347I //5 0x9595,240,320, LCD_DRIVER ID_7575
//#define LCD_MODEL ILI9486 //6 0x9486,320,480, LCD_DRIVER ID_9486
//#define LCD_MODEL ST7735S //7 0x7735,128,160, LCD_DRIVER ID_7735
//#define LCD_MODEL ILI9488 //8 0x9488,320,480, LCD_DRIVER ID_9488
//#define LCD_MODEL ILI9481 //9 0x9481,320,480, LCD_DRIVER ID_9481

// Set LCD System Interface 8 or 16 bit
#define LCD_SYS_INTERFACE  8
//#define LCD_SYS_INTERFACE  16

// Set CPU Frequency
// If you overclock, remove the comment below and enter the overclock value.
//#define TARGET_CPU_FREQ 96000000
#ifndef TARGET_CPU_FREQ 
#define TARGET_CPU_FREQ F_CPU
#endif

// Set LCD Pin definitions
// For maximum speed, the WR pin must be on the sequential DATAPORT.
#define D0 LL_GPIO_PIN_0   // PA0
#define D1 LL_GPIO_PIN_1   // PA1
#define D2 LL_GPIO_PIN_2   // PA2
#define D3 LL_GPIO_PIN_3   // PA3
#define D4 LL_GPIO_PIN_4   // PA4
#define D5 LL_GPIO_PIN_5   // PA5
#define D6 LL_GPIO_PIN_6   // PA6
#define D7 LL_GPIO_PIN_7   // PA7
#define WR LL_GPIO_PIN_8   // PA8
#define RS LL_GPIO_PIN_0   // PB0
#define CS LL_GPIO_PIN_1   // PB1
#define RD LL_GPIO_PIN_14  // PB14
#define RST LL_GPIO_PIN_15 // PB15
// For 16 bit lcd interface
#define D8 LL_GPIO_PIN_3   // PB3
#define D9 LL_GPIO_PIN_4   // PB4
#define D10 LL_GPIO_PIN_5  // PB5
#define D11 LL_GPIO_PIN_6  // PB6
#define D12 LL_GPIO_PIN_7  // PB7
#define D13 LL_GPIO_PIN_8  // PB8
#define D14 LL_GPIO_PIN_9  // PB9
#define D15 LL_GPIO_PIN_10 // PB10

// Set Touch Pins
// ILI9341 Touch pins are very important. If XM and YP are not analog pins, it will not work. 
// These correspond to XM:RS and YP:CS pins. XP and YM correspond to Dataport PA0 and PA1.
#define XM PB0 
#define YP PB1 
#define XP PA0 
#define YM PA1 