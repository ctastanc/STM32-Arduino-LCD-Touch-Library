#pragma once

#include <LCD_SRW.h>

void rotate_rect(void) { 
    const uint16_t colors[16] ={ BLUE, RED, GREEN, CYAN, MAGENTA,  YELLOW, WHITE, ORANGE,
        DARKGREEN, DARKCYAN,MAROON,PURPLE,OLIVE,LIGHTGREY, GREENYELLOW,PINK};
    lcd.Set_Rotation(LANDSCAPE);
    lcd.Cls(BLACK);
    while(true) {
        srand(time(0));
        int rn = colors[(rand() % 16)]; // int rn = 15+random(0xFFF0) ;
        lcd.Rectangle(0,0,lcd.Width,lcd.Height,YELLOW);
        lcd.Fast_HLine(35,120,245,BLUE);
        lcd.Fast_VLine(160,35,175,BLUE);
        lcd.Fill_Circle(160,120,40,BLACK);
        lcd.Circle(160,120,40,YELLOW);
        for(int z=1;z<3;z++) { 
            for(int x=1;x<91;x++) { 
                lcd.Fill_Rotated_Rectangle(160,120,50,50,x,rn);
                lcd.Fill_Rotated_Rectangle(80,60,50,50,90-x,rn);
                lcd.Fill_Rotated_Rectangle(240,180,50,50,90-x,rn);
                for(int i=2;i<5;i=i+2) {
                    lcd.Rotated_Rectangle(160,120,50+i,50+i,x,BLACK);
                    lcd.Rotated_Rectangle(80,60,50+i,50+i,90-x,BLACK);
                    lcd.Rotated_Rectangle(240,180,50+i,50+i,90-x,BLACK);
                }
            }
        }
        int rn1 = colors[(rand() % 16)];
        for(int x=90;x>=0;x--) {
            lcd.Fill_Rotated_Rectangle(160,120,50,50,x,rn1);
            lcd.Fill_Rotated_Rectangle(240,60,50,50,90-x,rn1);
            lcd.Fill_Rotated_Rectangle(80,180,50,50,90-x,rn1);
            for(int i=2;i<5;i=i+2) {
                lcd.Rotated_Rectangle(160,120,50+i,50+i,x,BLACK);
                lcd.Rotated_Rectangle(240,60,50+i,50+i,90-x,BLACK);
                lcd.Rotated_Rectangle(80,180,50+i,50+i,90-x,BLACK);
            }
        }
    }
}