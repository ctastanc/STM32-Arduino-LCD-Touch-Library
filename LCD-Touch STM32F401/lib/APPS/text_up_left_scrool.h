#pragma once

#include <LCD_SRW.h>

void text_LeftScrool() { 
    const char* text = "What will happen in the future is also one of the mysteries of the universe."\
                        "If you aspire to unravel the secrets of the universe,"\
                        "think in terms of energy, frequency, and vibrations.";
    lcd.Print_LeftScroll(text, lcd.Width, lcd.Height-125, 3, RED, BLACK, 10);
}

void text_UpScrool() {
    int  c [12] ={216,205,183,155,120,95,84,73,62,51,40,29};
    const tFont *ts[] ={&font9,&font19,&font19,&font30,&font19,&font9,&font9,&font9,&font9,&font9,&font9,&font9};
    uint16_t cl [12] ={CYAN,RED,GREEN,BLUE,RED,WHITE,YELLOW,GREEN,WHITE,GREEN,WHITE,GREEN};
    const char *s[12]={
        "Hello world!",
        "1234.56",
        "lcd ILI9341",
        "ENERGY",
        "In Universe.",
        "What will happen in the",
        "future is also one of the",
        "mysteries of the universe.",
        "If you aspire to unravel",
        "the secrets of the universe,",
        "think in terms of energy,",
        "frequency, and vibrations."
    };
    for (int j=0;j<12;j++){
        for(int i=1;i<c[j]+1;i++){
            lcd.Fill_Rect(0, lcd.Height-i+ts[j]->Height, lcd.Width - 1, 1, BLACK);
            lcd.PrintFont(*ts[j], s[j], CENTER, lcd.Height-i, 0, cl[j], BLACK, 1);
            delay(3);
        }
    } 
    delay(1000);
    for(int i=0;i<lcd.Height;i++){
        for (int j=0;j<12;j++){
            if (lcd.Height-c[j]-i>0) lcd.PrintFont(*ts[j], s[j], CENTER, lcd.Height-c[j]-i, 0, cl[j], BLACK, 1); 
                else lcd.Fill_Rect(0, lcd.Height-c[j]-i, lcd.Width - 1, ts[j]->Height, BLACK);
            lcd.Fill_Rect(0, lcd.Height-c[j]+ts[j]->Height-i, lcd.Width - 1, 1, BLACK);
            delayMicroseconds(300);
        }
    }
}