// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'

#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#include <LCD_KBV.h>
//touch sensitivity for x
#define TS_MINX 125//124
#define TS_MAXX 875//906
//touch sensitivity for Y
#define TS_MINY 85//83
#define TS_MAXY 895//893
//touch sensitivity for press
#define MINPRESSURE 350
#define MAXPRESSURE 1000

#define MAP(x,in_min,in_max,out_min,out_max) {(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min}
class TSPoint {
    public:
    TSPoint(void);
    TSPoint(int16_t x, int16_t y, int16_t z);
    bool operator==(TSPoint);
    bool operator!=(TSPoint);
    int16_t x, y, z;
};

class TouchScreen {
    public:
    TouchScreen();
    TouchScreen(uint16_t rx);
    TSPoint getPoint();
    int16_t pressureThreshhold;
    void pin_set(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l);
    private:
    uint8_t _yp, _ym, _xm, _xp;
    uint16_t _rxplate;
};

// Oversampling :
//   1 = no oversample 
//   2 = Double reading, if the two readings are not equal, consider it invalid (recommendation)
//   3+ = median value (insert sort)
#define NUMSAMPLES 2
TouchScreen ts=TouchScreen(295);

TSPoint::TSPoint(void) { x = y = 0; }
TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) { x = x0; y = y0; z = z0; }
bool TSPoint::operator==(TSPoint p1) { return ((p1.x == x) && (p1.y == y) && (p1.z == z)); }
bool TSPoint::operator!=(TSPoint p1) { return ((p1.x != x) || (p1.y != y) || (p1.z != z)); }

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
void TouchScreen::pin_set(uint8_t in1, uint8_t in2, uint8_t out1, uint8_t out2, uint8_t h, uint8_t l){
    pinMode(in1, INPUT);   pinMode(in2, INPUT);
    pinMode(out1, OUTPUT); digitalWrite(l, LOW);
    pinMode(out2, OUTPUT); digitalWrite(h, HIGH);
}
// STM32 note: analogRead must be in 10-bit mode. Add analogReadResolution(10) to setup().
TSPoint TouchScreen::getPoint(void) {
    int x, y, z;
    int samples[NUMSAMPLES];
    uint8_t i, valid = 1;
    pin_set(_yp,_xm,_xp,_ym,_ym,_xp); // Z (pressure) reading: XP=LOW, YM=HIGH, XM ve YP INPUT
    int z1 = analogRead(_xm);
    int z2 = analogRead(_yp);
    z = (1023 - (z2 - z1));
    if (z > MINPRESSURE && z < MAXPRESSURE) {
        pin_set(_ym,_yp,_xp,_xm,_xp,_xm); // X reading: XP=HIGH, XM=LOW; YP ve YM INPUT
        for (i = 0; i < NUMSAMPLES; i++) samples[i] = analogRead(_yp);
        #if NUMSAMPLES > 2
            insert_sort(samples, NUMSAMPLES);
        #endif
        #if NUMSAMPLES == 2
            if (samples[0] < samples[1]-2 || samples[0] > samples[1]+2) valid = 0; 
        #endif
        x = (1023 - samples[NUMSAMPLES/2]);
        pin_set(_xp,_xm,_yp,_ym,_yp,_ym); // Y reading: YP=HIGH, YM=LOW; XP ve XM INPUT
        for (i = 0; i < NUMSAMPLES; i++) samples[i] = analogRead(_xm);
        #if NUMSAMPLES > 2
            insert_sort(samples, NUMSAMPLES);
        #endif
        #if NUMSAMPLES == 2
            if (samples[0] < samples[1]-2 || samples[0] > samples[1]+2) valid = 0;
        #endif
        y = (1023 - samples[NUMSAMPLES/2]);
    } else z=0;
    if (!valid) z=0;
    pinMode(_xm, OUTPUT); pinMode(_xp, OUTPUT);
    int16_t y1;
    switch(lcd.rotation) {
		case 0: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height); break;
		case 1: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                y1 = y; y = lcd.Height-x; x = y1; break;
		case 2: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height);  
                x= lcd.Width-x; y=lcd.Height-y; break;
		case 3: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                y1 = y; y = x; x = lcd.Width-y1; break;
    }
    return TSPoint(x, y, z);
}

TouchScreen::TouchScreen() {
    _yp = YP; _xm = XM; _ym = YM; _xp = XP; _rxplate = 0; pressureThreshhold = 10;
}

TouchScreen::TouchScreen(uint16_t rxplate) {
    _yp = YP; _xm = XM; _ym = YM; _xp = XP; _rxplate = rxplate; pressureThreshhold = 10;
}

#endif
