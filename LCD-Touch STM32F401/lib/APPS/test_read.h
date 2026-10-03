#pragma once

#include <LCD_SRW.h>

void test_read() {
    const uint16_t color1[16] = { BLUE, RED, GREEN, CYAN, MAGENTA,  YELLOW, WHITE, ORANGE,
        DARKGREEN, DARKCYAN,MAROON,PURPLE,OLIVE,LIGHTGREY, GREENYELLOW,PINK};
    for(int i= 0; i<16;i++) {
        lcd.Pixel(50, i*15+10 ,color1[i]);
        int p=lcd.Read_Pixel(50,i*15+10);
        lcd.Print(p,70,i*15+5,1,p);
        lcd.Print((String)("ILI"+String(lcd.Read_ID(),HEX)),160,110,2,GREEN);
    }
}