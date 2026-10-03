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
    clrButton =  Buttons(lcd.Width/2-32,lcd.Height-35,64,30,BLUE,"Clear",WHITE,clrReleased);
    Button2 =  Buttons(lcd.Width/2-32,lcd.Height-239,64,30,BLUE,"Clear",WHITE,clrReleased);
    main_screen();
    while(true) {
        //int32_t start = micros();
        p = ts.getPoint();
        if (p.v) {
            print_coordinates(p);
            lcd.Pixel(p.x, p.y, RED);
            if( clrButton.isThis(p.x, p.y) ) {
                clrButton.Pressed();
                while(true) { p = ts.getPoint(); if(!p.z) break; }
                if(clrButton.isThis(p.x, p.y)) clrButton.Released(); else clrButton.Cancel();
            }
//Serial.print(p.z);Serial.print("-");Serial.print(p.v);Serial.print("-");Serial.print(p.x);Serial.print("-");Serial.println(p.y);
            if( Button2.isThis(p.x, p.y) ) {
                Button2.Pressed();
                while(true) { p = ts.getPoint(); if(!p.z) break; }
//Serial.print(p.z);Serial.print("-");Serial.print(p.v);Serial.print("-");Serial.print(p.x);Serial.print("-");Serial.println(p.y);
                if(Button2.isThis(p.x, p.y)) Button2.Released(); else Button2.Cancel();
            }
            //lcd.Print((micros()-start),lcd.Width/2-70,lcd.Height-100,2,RED,BLACK,1);
        }
    }
}