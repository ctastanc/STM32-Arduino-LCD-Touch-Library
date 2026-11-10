#pragma once

#include <touch_screen.h>
#include <buttons.h>

Buttons clrButton; 
Buttons Button2; 

void main_screen() {
    lcd.Cls(BLACK);
    lcd.Rectangle(0,0,lcd.Width,lcd.Height,YELLOW);
    clrButton.Draw();
    Button2.Draw();
}

void clrReleased() { main_screen(); }

void print_coordinates(TSPoint p) {
    lcd.Fill_Rectangle(lcd.Width/2-112,lcd.Height-25,80,16,BLACK);
    lcd.Fill_Rectangle(lcd.Width/2+40,lcd.Height-25,65,16,BLACK);
    lcd.Print(p.x,lcd.Width/2-112,lcd.Height-25,2,RED,BLACK,1);
    lcd.Print(p.y,lcd.Width/2-70,lcd.Height-25,2,RED,BLACK,1);
    lcd.Print(String("z:"+String(p.z)),lcd.Width/2+40,lcd.Height-25,2,RED,BLACK,1);
}

void touch_demo(void) {
    TSPoint p;
    clrButton = Buttons(ROUND,lcd.Width/2-32,lcd.Height-35,64,30,BLUE,font19,"Clear",WHITE,2,clrReleased);
    Button2 =   Buttons(ROUND,lcd.Width/2-32,lcd.Height-239,64,30,BLUE,font19,"Clear",WHITE,2,clrReleased);
    main_screen();
    while(true) {
        //int32_t start = micros();
        p = ts.getPoint();
        if (p.v) {
            print_coordinates(p);
            lcd.Pixel(p.x, p.y, RED);
            if( clrButton.isPress(p.x, p.y) ) {
                clrButton.Pressed();
                while(p.z) { p = ts.getPoint(); }
                if(clrButton.isPress(p.x, p.y)) clrButton.Released(); else clrButton.Cancel();
            }
            if( Button2.isPress(p.x, p.y) ) {
                Button2.Pressed();
                while(p.z) { p = ts.getPoint(); }
                if(Button2.isPress(p.x, p.y)) Button2.Released(); else Button2.Cancel();
            }
            //lcd.Print((micros()-start),lcd.Width/2-70,lcd.Height-100,2,RED,BLACK,1);
        }
    }
}