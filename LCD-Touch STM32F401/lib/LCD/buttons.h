#pragma once

#include <LCD_SRW.h>

class Buttons{

public:
    int x, y, w, h, b_color, name_color;
    void (*releasedFunc) ();
    void (*pressedFunc) ();
    const char* name;
    Buttons(){};
    Buttons(int x1, int y1, int w1, int h1, int b_color1, const char* name1, int name_color1, void func1()=NULL, void func2()=NULL){
        x=x1; y=y1; w=w1; h=h1; b_color=b_color1, name=name1, name_color=name_color1, releasedFunc=func1, pressedFunc=func2;
    }
    bool isThis(int x1, int y1) {
        if(x1>=x && x1<x+w && y1>y && y1<y+h) return true; else return false;
    }
    void Draw() {
        lcd.Fill_Rectangle(x,y,w,h,b_color);
        lcd.Print(name,x+5,y+(h/4),2,name_color);
    }
    void Pressed() {
        lcd.RectangleThickness(x,y,w,h,2,BLACK);
        if(pressedFunc != NULL) pressedFunc();
    }
    void Released() {
        lcd.RectangleThickness(x,y,w,h,2,b_color);
        if(releasedFunc != NULL) releasedFunc();
    }
    void Cancel() {
        lcd.RectangleThickness(x,y,w,h,2,b_color);
    }

};

