#include <touch_screen.h>

void main_screen();

void clrButton() {
    lcd.Fill_Rectangle(lcd.Width/2-32,lcd.Height-31,64,30,BLUE);
    lcd.Print("Clear",lcd.Width/2-27,lcd.Height-24,2,WHITE);
}

bool clrCheck(TSPoint p) { 
    if(p.x>=lcd.Width/2-30 && p.x<lcd.Width/2+30 && p.y >lcd.Height-30 && p.y<lcd.Height) return true; 
    else return false;
}

void clrPressed() { 
    lcd.Rectangle(lcd.Width/2-32,lcd.Height-31,64,30,BLACK);
    lcd.Rectangle(lcd.Width/2-31,lcd.Height-30,62,28,BLACK); 
}

void clrReleased() { main_screen(); }

void main_screen() {
    lcd.Fill_Screen(BLACK);
    lcd.Rectangle(0,0,lcd.Width,lcd.Height,YELLOW);
    clrButton();
}

void touch_demo(void) {
    main_screen();
    while(true) {
        TSPoint p = ts.getPoint();
        if (p.v) {
            lcd.Fill_Rectangle(lcd.Width/2-112,lcd.Height-25,80,16,BLACK);
            lcd.Fill_Rectangle(lcd.Width/2+40,lcd.Height-25,65,16,BLACK);
            lcd.Print(p.x,lcd.Width/2-112,lcd.Height-25,2,RED,BLACK,1);
            lcd.Print(p.y,lcd.Width/2-70,lcd.Height-25,2,RED,BLACK,1);
            lcd.Print(String("Z:"+String(p.z)),lcd.Width/2+40,lcd.Height-25,2,RED,BLACK,1);
            lcd.Pixel(p.x, p.y, RED);
            if(clrCheck(p)) {
                clrPressed();
                while(true) { p = ts.getPoint(); if(!p.z) break; }
                if(clrCheck(p)) clrReleased(); else clrButton();
            }
        }
    }
}