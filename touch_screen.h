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
    TSPoint(void) { x = y = 0; }
    TSPoint(int16_t x0, int16_t y0, int16_t z0,int16_t v0) { x = x0; y = y0; z = z0; v= v0; }
    TSPoint(int16_t z0, int16_t v0) { z= z0; v=v0; }
    int16_t x, y, z, v;
};

class TouchScreen {
    public:
    TouchScreen() { _yp = YP; _xm = XM; _ym = YM; _xp = XP; }
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    constexpr int32_t map_value(T1 x, T2 in_min, T3 in_max, T4 out_min, T5 out_max) {
        return (static_cast<int32_t>(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
    }

    void set_pin(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l) {
        digitalWrite(in1, LOW); pinMode(in1, INPUT);
        digitalWrite(in2, LOW); pinMode(in2, INPUT);
        pinMode(out1, OUTPUT);  pinMode(out2, OUTPUT); 
        digitalWrite(h, HIGH);  digitalWrite(l, LOW);
    }

    int getXY(uint8_t pin, bool &v ) {
        for(int i = 0; i < NUMSAMPLES; i++) samples[i] = analogRead(pin); 
        if(samples[0] < samples[1]-3 || samples[0] > samples[1]+3) v = 0; 
        return (RES_VALUE - samples[NUMSAMPLES/2]); 
    }

    // For calibration
    TSPoint getRawPoint(void) {
        uint16_t rx, ry, rz; bool v = 1;
        set_pin(_ym, _yp, _xp, _xm, _xp, _xm); 
        rx = getXY(_yp, v);
        set_pin(_xp, _xm, _yp, _ym, _yp, _ym); 
        ry = getXY(_xm, v);
        set_pin(_yp, _xm, _xp, _ym, _ym, _xp); 
        int z1 = analogRead(_xm);
        int z2 = analogRead(_yp);
        rz = (RES_VALUE - (z2 - z1));
        pinMode(_xm, OUTPUT); pinMode(_yp, OUTPUT); /*pinMode(_xp, OUTPUT); pinMode(_ym, OUTPUT);*/ 
        if( rz<20 ) v = 0;
        if(v) return TSPoint(rx, ry, rz, v); else return TSPoint(rz, v);
    }
    
    TSPoint getPoint(void) {
        uint16_t x, y, z; bool v = 1;
        set_pin(_ym,_yp,_xp,_xm,_xp,_xm); 
        x = getXY(_yp, v);
        set_pin(_xp,_xm,_yp,_ym,_yp,_ym); 
        y = getXY(_xm, v);
        set_pin(_yp,_xm,_xp,_ym,_ym,_xp); 
        int z1 = analogRead(_xm);
        int z2 = analogRead(_yp);
        z = (RES_VALUE - (z2 - z1)); // pressure
        pinMode(_xm, OUTPUT); pinMode(_yp, OUTPUT); RS_DATA;/*pinMode(_xp, OUTPUT); pinMode(_ym, OUTPUT);*/ 

        int32_t rx = map_value(x, TS_MINX, TS_MAXX, 0, (lcd.rotation % 2 == 0) ? lcd.Width : lcd.Height);
        int32_t ry = map_value(y, TS_MINY, TS_MAXY, 0, (lcd.rotation % 2 == 0) ? lcd.Height : lcd.Width);
        struct Point { int32_t x; int32_t y; };
        Point rot_table[4] = { { rx, ry }, { ry, lcd.Height - rx }, { lcd.Width - rx, lcd.Height - ry }, 
            { lcd.Width - ry, rx } };
        x = rot_table[lcd.rotation].x; 
        y = rot_table[lcd.rotation].y;

        if( x>lcd.Width || y>lcd.Height ) v = 0; 
        if(z<50) v=z=0; // z should be tested independently of x and y. Because if x or y is false, z will be "0" as well.
        if(v) return TSPoint(x, y, z, v); else return TSPoint(z, v);
    }

    private:
    int samples[NUMSAMPLES];
    uint8_t _yp, _ym, _xm, _xp;
};

TouchScreen ts=TouchScreen();
