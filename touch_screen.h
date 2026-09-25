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
#define MINPRESSURE 0 //touch sensitivity for press
#define MAXPRESSURE 4000
#define RES_VALUE 4095

#define MAP(x,in_min,in_max,out_min,out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

// Oversampling :
//   1 = no oversample 
//   2 = Double reading, if the two readings are not equal, consider it invalid (recommendation)
//   3+ = median value (insert sort)
#define NUMSAMPLES 2

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
    uint8_t j;
    int save;
    for (int i = 1; i < size; i++) {
        save = array[i];
        for (j = i; j >= 1 && save < array[j-1]; j--)
        array[j] = array[j-1];
        array[j] = save;
    }
}
#endif

class TSPoint {
    public:
    TSPoint(void) { x = y = v = 0; }
    TSPoint(int16_t x0, int16_t y0, int16_t v0,int16_t z0) { x = x0; y = y0; v = v0; z= z0; }
    bool operator==(TSPoint p1) { return ((p1.x == x) && (p1.y == y) && (p1.v == v)); }
    bool operator!=(TSPoint p1) { return ((p1.x != x) || (p1.y != y) || (p1.v != v)); }
    int16_t x, y, v, z;
};

class TouchScreen {
    public:
    TouchScreen() {
        _yp = YP; _xm = XM; _ym = YM; _xp = XP; _rxplate = 0;
    }

    TouchScreen(uint16_t rxplate) {
        _yp = YP; _xm = XM; _ym = YM; _xp = XP; _rxplate = rxplate; 
    }
    
    void pin_set(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l) {
        pinMode(in1, INPUT);   pinMode(in2, INPUT);
        pinMode(out1, OUTPUT); digitalWrite(l, LOW);
        pinMode(out2, OUTPUT); digitalWrite(h, HIGH);
    }
    
    TSPoint getPoint(void) {
        int x, y, v, z;
        int samples[NUMSAMPLES];
        uint8_t i, valid = 1;
        v = 1;
        pin_set(_yp,_xm,_xp,_ym,_ym,_xp); // Z (pressure) reading: XP=LOW, YM=HIGH, XM ve YP INPUT
        int z1 = analogRead(_xm);
        int z2 = analogRead(_yp);
        z = (RES_VALUE - (z2 - z1)); // pressure

            pin_set(_ym,_yp,_xp,_xm,_xp,_xm); // X reading: XP=HIGH, XM=LOW; YP ve YM INPUT
            for (i = 0; i < NUMSAMPLES; i++) {samples[i] = analogRead(_yp);}
            #if NUMSAMPLES > 2
                insert_sort(samples, NUMSAMPLES);
            #endif
            #if NUMSAMPLES == 2
                //if (samples[0] != samples[1]) { v = 0; }
                if (samples[0] < samples[1]-1 || samples[0] > samples[1]+1) v = 0; 
            #endif
            x = (RES_VALUE - samples[NUMSAMPLES/2]); 
            
            pin_set(_xp,_xm,_yp,_ym,_yp,_ym); // Y reading: YP=HIGH, YM=LOW; XP ve XM INPUT
            for (i = 0; i < NUMSAMPLES; i++) {samples[i] = analogRead(_xm);}
            #if NUMSAMPLES > 2
                insert_sort(samples, NUMSAMPLES);
            #endif
            #if NUMSAMPLES == 2
                //if (samples[0] != samples[1]) { v = 0; }
                if (samples[0] < samples[1]-1 || samples[0] > samples[1]+1) v = 0;
            #endif
            y = (RES_VALUE - samples[NUMSAMPLES/2]); 

        pinMode(_xm, OUTPUT); pinMode(_xp, OUTPUT); pinMode(_ym, OUTPUT); pinMode(_yp, OUTPUT);
        int16_t y1;
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
                    x= lcd.Width-x; y=lcd.Height-y; 
                    break;
            case 3: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                    y1 = y; y = x; x = lcd.Width-y1; 
                    break;
        }
        if(x<0 || x>lcd.Width || y>lcd.Height || y<0) v=0;
        return TSPoint(x, y, v, z);
    }

    private:
    uint8_t _yp, _ym, _xm, _xp;
    uint16_t _rxplate;
};

TouchScreen ts=TouchScreen(295);

