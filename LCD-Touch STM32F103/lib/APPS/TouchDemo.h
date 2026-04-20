#include <TouchScreen.h>

void touch_demo(void);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts1 = TouchScreen(295);

void touch_demo(void) {
    // a point object holds x y and z coordinates
    lcd.Fill_Screen(BLACK);
    lcd.Rectangle(0,0,lcd.Width-1,lcd.Height-1,YELLOW);
    while(1){
        TSPoint p = ts.getPoint();
        // we have some minimum pressure we consider 'valid'
        // pressure of 0 means no pressing!
        if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            Serial.print("\tPressure = "); Serial.print(p.z);
            Serial.print("   X = "); Serial.print(p.x);
            Serial.print("\tY = "); Serial.println(p.y);
            lcd.Print("*",p.x,p.y,2,RED);
        }
    }
}