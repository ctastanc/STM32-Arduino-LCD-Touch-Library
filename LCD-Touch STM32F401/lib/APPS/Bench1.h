#include <LCD_SRW.h> 

class ben1{
    public:
    uint16_t testFillScreen() {
        lcd.Fill_Screen(RED);
        delay(500);
        lcd.Fill_Screen(GREEN);
        delay(500);
        lcd.Fill_Screen(BLUE);
        delay(500);
        uint16_t start = micros();
        lcd.Fill_Screen(BLACK);
        return micros() - start;
    }

    uint16_t testTextFR() {
        lcd.Fill_Screen(BLACK);
        uint16_t start = micros();
        lcd.Print("Hello World!", CENTER, 24, 1, WHITE,BLUE,0);
        lcd.Print(1234.56, CENTER, 35, 2, YELLOW,BLUE,0);
        lcd.Print("lcd ILI9341", CENTER, 55, 2, RED,BLUE,0);
        lcd.Print("ENERGY", CENTER, 85, 4, GREEN,BLUE,0);
        lcd.Print("In Universe.", CENTER, 130, 2, GREEN,BLUE,0);
        lcd.Print("What will happen in the", CENTER, 150, 1, RED,BLUE,0);
        lcd.Print("future is also one of the", CENTER, 160, 1, WHITE,BLUE,0);
        lcd.Print("mysteries of the universe.", CENTER, 170, 1, GREEN,BLUE,0);
        lcd.Print("If you aspire to unravel", CENTER, 180, 1, GREEN,BLUE,0);
        lcd.Print("the secrets of the universe,", CENTER, 190, 1, GREEN,BLUE,0);
        lcd.Print("think in terms of energy,", CENTER, 200, 1, GREEN,BLUE,0);
        lcd.Print("frequency, and vibrations.", CENTER, 210, 1, GREEN,BLUE,0); 
        uint16_t t = micros() - start;
        //lcd.Print("see if I don't!", CENTER, 238, 1, GREEN,BLUE,0); 
        return t;
    }

    uint16_t testTextBG() {
        lcd.Fill_Screen(BLACK);
        uint16_t start = micros();
        lcd.Print("Hello World!", CENTER, 24, 1, WHITE,BLUE,1);
        lcd.Print(1234.56, CENTER, 35, 2, YELLOW,BLUE,1);
        lcd.Print("lcd ILI9341", CENTER, 55, 2, RED,BLUE,1);
        lcd.Print("ENERGY", CENTER, 85, 4, GREEN,BLUE,1);
        lcd.Print("In Universe.", CENTER, 130, 2, GREEN,BLUE,1);
        lcd.Print("What will happen in the", CENTER, 150, 1, RED,BLUE,1);
        lcd.Print("future is also one of the", CENTER, 160, 1, WHITE,BLUE,1);
        lcd.Print("mysteries of the universe.", CENTER, 170, 1, GREEN,BLUE,1);
        lcd.Print("If you aspire to unravel", CENTER, 180, 1, GREEN,BLUE,1);
        lcd.Print("the secrets of the universe,", CENTER, 190, 1, GREEN,BLUE,1);
        lcd.Print("think in terms of energy,", CENTER, 200, 1, GREEN,BLUE,1);
        lcd.Print("frequency, and vibrations.", CENTER, 210, 1, GREEN,BLUE,1); 
        uint16_t t = micros() - start;
        //lcd.Print("see if I don't!", CENTER, 238, 1, GREEN,BLUE,1); 
        return t;
    }

    uint16_t testLines(uint16_t color) {
        uint16_t start, t;
        int x1, y1, x2, y2, d, w = lcd.Width, h = lcd.Height;
        d=200;
        lcd.Fill_Screen(BLACK);
        x1 = y1 = 0;  y2 = h - 1;
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, color);
        x2 = w - 1;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        delay(d);

        lcd.Fill_Screen(RED);
        x1 = w - 1;  y1 = 0;  y2 = h - 1;
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, color);
        x2 = 0;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        delay(d);

        lcd.Fill_Screen(GREEN);
        x1 = 0;  y1 = h - 1;  y2 = 0;
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, color);
        x2 = w - 1;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        delay(d);

        lcd.Fill_Screen(BLUE);
        x1 = w - 1;  y1 = h - 1;  y2 = 0;
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, color);
        x2 = 0;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        start = micros();
        lcd.Line(0, 0, w-1, h-1, CYAN);
        return micros() - start;
    }

    uint16_t testFastLines(uint16_t color1, uint16_t color2) {
        uint16_t start;
        int x, y, w = lcd.Width, h = lcd.Height;
        lcd.Fill_Screen(BLACK);
        start = micros();
        lcd.Fast_HLine(0, (h-1)/2, w-1, color1);
        int t =micros() - start;
        for(y=10; y<h; y+=10) {lcd.Fast_HLine(0, y, w, color1); }
        delay(500);
        for(x=10; x<w; x+=10) {lcd.Fast_VLine(x, 0, h, color1); }
        return t;
    }

    uint16_t testRects(uint16_t color) {
        uint16_t start;
        int n, i, i2, w = lcd.Width, h = lcd.Height;
        lcd.Fill_Screen(BLACK);
        n = min(lcd.Width, lcd.Height);
        for(i=2; i<n; i+=10) {
            i2 = i / 2;
            lcd.Rectangle_XY(i2, i2, w-i2, h-i2, color);
        }
        start = micros();
        lcd.Rectangle_XY(0, 0, w-1, h-1, YELLOW);
        return micros() - start;
    }

    uint16_t testFilledRects(uint16_t color1, uint16_t color2) {
        uint16_t start, t = 0;
        int n, i, i2, w = lcd.Width, h = lcd.Height;
        lcd.Fill_Screen(BLACK);
        n = min(lcd.Width, lcd.Height);
        for(i=n; i>0; i-=6) {
            i2    = i / 2;
            lcd.Fill_Rectangle_XY(i2, i2, w-i2, h-i2, color1);
        }
        start = micros();
        lcd.Fill_Rectangle_XY(0, 0, lcd.Width-1, lcd.Height-1, RED);
        return micros() - start;
    }

    uint16_t testFilledCircles(uint8_t radius, uint16_t color) {
        uint16_t start;
        int x, y, w = lcd.Width, h = lcd.Height, r2 = radius * 2;
        lcd.Fill_Screen(BLACK);
        for(x=radius; x<w; x+=r2) {
            for(y=radius; y<h; y+=r2) {
            lcd.Fill_Circle(x, y, radius, color);
            }
        }
        start = micros();
        lcd.Fill_Circle(lcd.Width/2, lcd.Height/2, 119, RED);
        return micros() - start;
    }

    uint16_t testCircles(uint8_t radius, uint16_t color) {
        uint16_t start;
        int x, y, r2 = radius * 2, w = lcd.Width  + radius-1, h = lcd.Height + radius-1;
        for(x=0; x<w; x+=r2) {
            for(y=0; y<h; y+=r2) {
            lcd.Circle(x, y, radius, color);
            }
        }
        start = micros();
        lcd.Circle(lcd.Width/2, lcd.Height/2, 119, color);
        return micros() - start;
    }

    uint16_t testTriangles() {
        uint16_t start;
        int n, i, cx = lcd.Width  / 2 , cy = lcd.Height / 2+40 ;
        lcd.Fill_Screen(BLACK);
        n = min(cx, cy);
        for(i=0; i<n; i+=10) {
            lcd.Triangle(
            cx    , cy - i, cx - i, cy + i/2, cx + i, cy + i/2, NAVY);
        }
        start = micros();
        lcd.Triangle(cx, 0, cx - cx, lcd.Height, lcd.Width, lcd.Height, YELLOW);
        return micros() - start;
    }

    uint16_t testFilledTriangles() {
        uint16_t start, t = 0;
        int i, cx = lcd.Width  / 2 - 1, cy = lcd.Height / 2 - 1;
        lcd.Fill_Screen(BLACK);
        for(i=min(cx,cy); i>0; i-=1) {
            lcd.Fill_Triangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, YELLOW);
            lcd.Triangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, MAROON);
        }
        start = micros();
        lcd.Fill_Triangle(cx, cy-cy, cx - cx, cy + cy, cx + cx, cy + cy, YELLOW);
        return micros() - start;
    }

    uint16_t testRoundRects() {
        uint16_t start;
        int n, i, i2, w = lcd.Width-1, h = lcd.Height-1;
        lcd.Fill_Screen(BLACK);
        n = min(lcd.Width, lcd.Height);
        for(i=0; i<n-4; i+=6) {
            i2 = i/2;
            lcd.Round_Rectangle_XY(i2-1, i2-1, w-i2, h-i2, 4, PINK);
        }
        start = micros();
        lcd.Round_Rectangle_XY(0, 0, lcd.Width-1,lcd.Height-1, 4, PINK);
        return micros() - start;
    }

    uint16_t testFilledRoundRects() {
        uint16_t start;
        int n, i, i2, w = lcd.Width-1, h = lcd.Height-1;
        n = min(lcd.Width, lcd.Height);
        lcd.Fill_Screen(BLACK);
        for(i=n; i>=0; i-=6) {
            i2 = i / 2;
            lcd.Fill_Round_Rectangle_XY(i2, i2, w-i2, h-i2, 4, DARKCYAN);
        }
        start = micros();
        lcd.Fill_Round_Rectangle_XY(0, 0, lcd.Width-1,lcd.Height-1, 4, DARKCYAN);
        return micros() - start;
    }
    //extern int m;
    void bench1() {
            lcd.Set_Rotation(LANDSCAPE);
            uint16_t d=1000;
            //Serial.println(String(SystemCoreClock/1000000)+"Mhz");
            //Serial.println(F("Benchmark                Time (microseconds)"));
            //Serial.print(F("Screen fill              "));
            //Serial.println(testFillScreen());

            Serial.print(F("Text FR                  "));
            Serial.println(testTextFR());
            delay(d);

            //Serial.println(m);
            Serial.print(F("Text BG                  "));
            Serial.println(testTextBG());
        
            delay(d);
            /*Serial.print(F("Lines                    "));
            Serial.println(testLines(CYAN));
            delay(d);

            Serial.print(F("Horiz/Vert Lines         "));
            Serial.println(testFastLines(RED, BLUE));
            delay(d);

            Serial.print(F("Rectangles (outline)     "));
            Serial.println(testRects(GREEN));
            delay(d);

            Serial.print(F("Rectangles (filled)      "));
            Serial.println(testFilledRects(YELLOW, MAGENTA));
            delay(d);

            Serial.print(F("Circles (filled)         "));
            Serial.println(testFilledCircles(10, MAGENTA));
            delay(d);

            Serial.print(F("Circles (outline)        "));
            Serial.println(testCircles(10, WHITE));
            delay(d);

            Serial.print(F("Triangles (outline)      "));
            Serial.println(testTriangles());
            delay(d);

            Serial.print(F("Triangles (filled)       "));
            Serial.println(testFilledTriangles());
            delay(d);

            Serial.print(F("Rounded rects (outline)  "));
            Serial.println(testRoundRects());
            delay(d);

            Serial.print(F("Rounded rects (filled)   "));
            Serial.println(testFilledRoundRects());
            delay(d);*/

            Serial.println(F("Done!"));
    }
};
ben1 b1;