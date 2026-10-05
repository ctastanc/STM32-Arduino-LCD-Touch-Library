#pragma once
/********************************************************************************** 
    ATTENTION:
    To ensure the ADC operates cleanly and accurately, keep the USB power cable and 
    other cables as short as possible, and ensure there is no leakage in the cabling.

************************************************************************************/
#include <LCD_SRW.h>

#define TS_MINX 450//500 //touch sensitivity for x
#define TS_MAXX 3549//3500
#define TS_MINY 299//350 //touch sensitivity for Y
#define TS_MAXY 3592//3600
#define RES_VALUE 4095
#define NUMSAMPLES 2

class TSPoint {
    public:
    TSPoint(void) : x(0), y(0) {}
    TSPoint(int16_t x0, int16_t y0, int16_t z0,int16_t v0) : x(x0), y(y0), z(z0), v(v0) {}
    TSPoint(int16_t z0, int16_t v0) : z(z0), v(v0) {}
    int16_t x, y, z, v;
};

class TouchScreen {
    private:
    int samples[NUMSAMPLES];
    uint8_t yp, ym, xm, xp;

    public:
    TouchScreen() : yp(YP), xm(XM), ym(YM), xp(XP) { }
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    constexpr int32_t map_value(T1 x, T2 in_min, T3 in_max, T4 out_min, T5 out_max) {
        return (static_cast<int32_t>(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
    }

    struct Points{ int16_t x; int16_t y; int16_t z; bool v;} p;

    void set_pin(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l) {
        digitalWrite(in1, LOW); pinMode(in1, INPUT);
        digitalWrite(in2, LOW); pinMode(in2, INPUT);
        pinMode(out1, OUTPUT);  pinMode(out2, OUTPUT); 
        digitalWrite(h, HIGH);  digitalWrite(l, LOW);
    }

    int getXY(uint8_t pin) {
        for(int i = 0; i < NUMSAMPLES; i++) samples[i] = analogRead(pin); 
        if(samples[0] < samples[1]-3 || samples[0] > samples[1]+3) p.v = 0; 
        return (RES_VALUE - samples[NUMSAMPLES/2]); 
    }

    void getRaw(void) {
        p.v = 1;
        set_pin(ym, yp, xp, xm, xp, xm); 
        p.x = getXY(yp);
        set_pin(xp, xm, yp, ym, yp, ym); 
        p.y = getXY(xm);
        set_pin(yp, xm, xp, ym, ym, xp); 
        int z1 = analogRead(xm);
        int z2 = analogRead(yp);
        p.z = (RES_VALUE - (z2 - z1));
        pinMode(xm, OUTPUT); pinMode(yp, OUTPUT); RS_DATA;
    }

    TSPoint getRawPoint(void) {
        getRaw();
        if(p.z<20) p.v = 0;
        if(p.v) return TSPoint(p.x, p.y, p.z, p.v); else return TSPoint(p.z, p.v);
    }
    
    TSPoint getPoint(void) {
        getRaw();
        int32_t rx = map_value(p.x, TS_MINX, TS_MAXX, 0, (lcd.rotation % 2 == 0) ? lcd.Width : lcd.Height);
        int32_t ry = map_value(p.y, TS_MINY, TS_MAXY, 0, (lcd.rotation % 2 == 0) ? lcd.Height : lcd.Width);
        struct Point { int32_t x; int32_t y; };
        Point rot_table[4] = { { rx, ry }, { ry, lcd.Height - rx }, { lcd.Width - rx, lcd.Height - ry }, 
                               { lcd.Width - ry, rx } };
        p.x = rot_table[lcd.rotation].x; 
        p.y = rot_table[lcd.rotation].y;

        if(p.x>lcd.Width || p.y>lcd.Height) p.v = 0; 
        if(p.z<50) p.v=p.z=0; // z should be tested independently of x and y. Because if x or y is false, z will be "0" as well.
        if(p.v) return TSPoint(p.x, p.y, p.z, p.v); else return TSPoint(p.z, p.v);
    }
};

TouchScreen ts=TouchScreen();
