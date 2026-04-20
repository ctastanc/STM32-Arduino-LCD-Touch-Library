/***********************************************************************************
*This program is a demo of how to use the touch function in a phone GUI
*This demo was made for LCD modules with 8bit data port.
* STM32F103C8T6 (Blue Pill) - LCD 8-bit parallel LCD + TouchScreen
* Hardware Environment: STM32 Arduino STM32F103x
* Build Environment   : STM32Arduino
*
*=========================== pin usage as follow: ================================
*                  LCD_CS  LCD_RS  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK
*     STM32F103      B1      B0      PA8    PB14    PB15
*     Touch          YP      XM
*
*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7
*     STM32F103      PA0     PA1     PA2     PA3     PA4     PA5     PA6     PA7
*     Touch           XP     YM
*=================================================================================
*
*----------------- Pins and ports are defined in mcu_regs.h ----------------
*
* NOTE: PB3 and PB4 are JTAG pins. They can be used freely in SWD mode (default).
**********************************************************************************/

#include <LCD_KBV.h> 
#include <sin_cos.h>
#include <phonecall.h>
#include <touchdemo.h>
#include <read_pixel.h>
#include <switch_test.h>
#include <colligate_test.h>

void full_screen_test() {
    uint32_t start = micros(); 
    lcd.Fill_Screen(BLUE); 
    uint32_t elapsed_us = micros() - start;
    Serial.println(String(elapsed_us)+ " us"); 
    digitalWrite(PC13, HIGH);
    lcd.Print((String)("MCU "+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,YELLOW,BLUE,0);
    lcd.Print((String)(String(elapsed_us)+" us"),CENTER,130,4,YELLOW,BLUE,0);
    lcd.Print((String)("APB1 "+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,205,3,YELLOW,BLACK,0);
    delay(1000);
    lcd.Fill_Screen(BLACK); 
    digitalWrite(PC13, LOW);
    lcd.Print((String)("MCU "+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,YELLOW,BLUE,0);
    lcd.Print((String)(String(elapsed_us)+" us"),CENTER,130,4,YELLOW,BLUE,0);
    lcd.Print((String)("APB1 "+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,205,3,YELLOW,BLACK,0);
    delay(1000);
}

void setup(void) {
    Serial.begin(115200);
    delay(150); 
    Serial.println("system running...");
    analogReadResolution(10); // STM32 ADC default 12-bit; touch 10-bit
    pinMode(PC13,OUTPUT);
    lcd.Init_LCD();
}

void loop(void) {
    //full_screen_test();
    //fast_sin_cos();
    colligate();
    //touch_demo();
    //phonecall();
    //rp.read_pixel();
    //sw.switch_test();
}
