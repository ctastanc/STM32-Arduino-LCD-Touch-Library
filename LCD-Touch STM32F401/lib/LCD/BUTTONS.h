#pragma once

#include <LCD_SRW.h>

#define FLAT 0
#define ROUND 1
#define CIRCULAR 2

class Dual_Delay{
    public:
    int firstDelay, secondDelay, countDelay; bool first;
    Dual_Delay(){}
    Dual_Delay(int f, int s, bool fs) : firstDelay(f), secondDelay(s), first(fs) {}
    void set_delay() {countDelay=millis()+firstDelay; first=1;}
    byte get_delay() {if(millis() > countDelay) {countDelay=millis()+secondDelay; first=0; return 1;} return 0;}
};

class Buttons{
    public:
    int  type, x, y, w, h, color, name_color, name_size;
    const char* name;
    tFont font;
    void (*releasedFunc) ();
    void (*pressedFunc) ();
    
    Buttons(){};
    Buttons(int type, int x, int y, int w, int h, int color, const tFont &pfont, 
        const char* name, int name_color, int name_size, void releasedFunc()=NULL, void pressedFunc()=NULL) :
        type(type), x(x), y(y), w(w), h(h), color(color), font(pfont), name(name), name_color(name_color), name_size(name_size),
        releasedFunc(releasedFunc), pressedFunc(pressedFunc) {}
       
    bool isPress(int x1, int y1) {
        if(x1>=x && x1<x+w && y1>y && y1<y+h) return true; else return false;
    }
    void Draw() {
        int str_len=strlen(name);//(sizeof(name)/sizeof(name[0]));
        int name_w =0;
        int uch = font.UpperCaseBottomSpc;
        for(int i=0; i<str_len; i++) {
            name_w += font.Offset[name[i]-32][0]+font.CharSpace;
            if(name[i]=='g' | name[i]=='j' | name[i]=='p' | name[i]=='q' | name[i]=='y') uch=0;
        }
        int name_h =font.Height-uch;
        int name_x=x+(w-name_w)/2;
        int name_y=y+(h-name_h)/2;
        if(type==FLAT) lcd.Fill_Rectangle(x,y,w,h,color);
        else if(type==ROUND) lcd.Fill_Round_Rectangle(x,y,w,h,8,color);
        else if(type==CIRCULAR) { 
            if(w >h) h=w; else w=h; 
            name_x=x+(w-name_w)/2;
            name_y=y+(h-name_h)/2;
            lcd.Fill_Circle(x+w/2,y+(h/2),h/2,color);
        }
        lcd.PrintFont(font,name,name_x,name_y,name_size,name_color);
    }
    void Pressed() {
        if(type==FLAT) lcd.Rectangle_Thickness(x,y,w,h,2,BLACK);
        else if(type==ROUND) lcd.Round_Rectangle_Thickness(x,y,w,h,8,2,BLACK);
        else if(type==CIRCULAR) lcd.Circle_Thickness(x+w/2,y+(h/2),h/2,2,BLACK);
        if(pressedFunc != NULL) pressedFunc();
    }
    void Released() {
        if(releasedFunc != NULL) releasedFunc(); else Draw();
    }
    void Cancel() {
        Draw();
    }
};

