#include <touch_screen.h>

void touch_demo(void);

void clrButton();

void scr(){
    lcd.Fill_Screen(BLACK);
    lcd.Rectangle(0,0,lcd.Width,lcd.Height,YELLOW);
    clrButton();
}

bool checkClr(TSPoint p) { 
    if(p.x>=lcd.Width/2-30 && p.x<lcd.Width/2+30 && p.y >lcd.Height-30 && p.y<lcd.Height) return true; 
    else return false;
}

void clrButton() {
    lcd.Fill_Rectangle(lcd.Width/2-32,lcd.Height-31,64,30,BLUE);
    lcd.Print("Clear",lcd.Width/2-27,lcd.Height-24,2,WHITE);
}

void clrPressed() { 
    lcd.Rectangle(lcd.Width/2-32,lcd.Height-31,64,30,BLACK);
    lcd.Rectangle(lcd.Width/2-31,lcd.Height-30,62,28,BLACK); 
}

void clrReleased() { scr(); }

void touch_demo(void) {
    lcd.Set_Rotation(LANDSCAPE);
    scr();
    while(1){
        TSPoint p = ts.getPoint();
        if (p.v) {
            Serial.print("\tPressure = "); Serial.print(p.z);
            Serial.print("   X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.println(p.y);
            lcd.Fill_Rectangle(15,lcd.Height-25,33,16,BLACK);
            lcd.Fill_Rectangle(75,lcd.Height-25,33,16,BLACK);
            lcd.Print(p.x,15,lcd.Height-25,2,RED,BLACK,1);
            lcd.Print(p.y,75,lcd.Height-25,2,RED,BLACK,1);
            lcd.Pixel(p.x, p.y, RED);
            if(checkClr(p)){
                clrPressed();
                while(1) { 
                    p = ts.getPoint(); 
                    if(!p.z) break; 
                }
                if(checkClr(p)) clrReleased(); else clrButton();
                
            }
        }
    }
}