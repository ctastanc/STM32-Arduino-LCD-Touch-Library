#include <touch_screen.h>

void touch_demo(void);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
// TouchScreen ts1 = TouchScreen(295);

void scr(){
    lcd.Fill_Screen(BLACK);
    lcd.Rectangle(0,0,lcd.Width,lcd.Height,YELLOW);
    lcd.Fill_Rectangle(lcd.Width/2-30,lcd.Height-30,60,29,BLUE);
    lcd.Print("Clear",lcd.Width/2-27,lcd.Height-24,2,WHITE);
}
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
            if(p.x>=lcd.Width/2-30 && p.x<lcd.Width/2+30 && p.y >lcd.Height-30 && p.y<lcd.Height){
                scr();
                while(1){TSPoint p = ts.getPoint(); if(!p.z) break;}
            }
        }
    }
}