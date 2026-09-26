#pragma once
/********************************************************************************** 
    ATTENTION:
    To ensure the ADC operates cleanly and accurately, keep the USB power cable and 
    other cables as short as possible, and ensure there is no leakage in the cabling.

************************************************************************************/
#include <LCD_SRW.h>

#define TS_MINX 500 //touch sensitivity for x
#define TS_MAXX 3500
#define TS_MINY 350 //touch sensitivity for Y
#define TS_MAXY 3600
#define RES_VALUE 4095

#define MAP(x,in_min,in_max,out_min,out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
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

    void pin_set(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l) {
        digitalWrite(in1, LOW); pinMode(in1, INPUT);
        digitalWrite(in2, LOW); pinMode(in2, INPUT);
        pinMode(out1, OUTPUT); digitalWrite(l, LOW);
        pinMode(out2, OUTPUT); digitalWrite(h, HIGH);
    }

    int getXY(uint8_t pin, uint16_t &v ) {
        for (int i = 0; i < NUMSAMPLES; i++) {samples[i] = analogRead(pin);}
        if (samples[0] < samples[1]-1 || samples[0] > samples[1]+1) v = 0; 
        return (RES_VALUE - samples[NUMSAMPLES/2]); 
    }
    
    TSPoint getPoint(void) {
        uint16_t x, y, z, v = 1, y1;
        pin_set(_ym,_yp,_xp,_xm,_xp,_xm); 
        x = getXY(_yp, v);
        pin_set(_xp,_xm,_yp,_ym,_yp,_ym); 
        y = getXY(_xm, v);
        pin_set(_yp,_xm,_xp,_ym,_ym,_xp); 
        int z1 = analogRead(_xm);
        int z2 = analogRead(_yp);
        z = (RES_VALUE - (z2 - z1)); // pressure
        pinMode(_xm, OUTPUT); pinMode(_xp, OUTPUT); pinMode(_ym, OUTPUT); pinMode(_yp, OUTPUT);
        switch(lcd.rotation) {
            case 0: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height); 
                    break;
            case 1: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                    y1 = y; y = lcd.Height-x; x = y1; 
                    break;
            case 2: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height);  
                    x = lcd.Width-x; y = lcd.Height-y; 
                    break;
            case 3: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                    y1 = y; y = x; x = lcd.Width-y1; 
                    break;
        }
        if( x>lcd.Width || y>lcd.Height || z<20 ) v = 0;
        if(v) return TSPoint(x, y, z, v); else return TSPoint(z, v);
    }

    private:
    int samples[NUMSAMPLES];
    uint8_t _yp, _ym, _xm, _xp;
};

TouchScreen ts=TouchScreen();
