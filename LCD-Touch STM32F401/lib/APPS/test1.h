/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <LCD_SRW.h> 

class Test{
    public:
    long testFillScreen() {
        lcd.Fill_Screen(BLACK);
        long start = micros();
        lcd.Fill_Screen(BLUE);
        return micros() - start;
    }

    long testText_fr() {
        lcd.Cls(BLACK);
        long start = micros();
        lcd.Print("Hello World!",0,0,1,WHITE);
        lcd.Print(1234.56,0,10,2,YELLOW);
        lcd.Print("lcd is ILI9341",0,30,2,RED,0,0,16);
        lcd.Print("Groop",0,60,5,GREEN);
        lcd.Print("I implore thee,",0,110,2,GREEN);
        lcd.Print("my foonting turlingdromes.",0,130,1,GREEN);
        lcd.Print("And hooptiously drangle me",0,140,1,GREEN);
        lcd.Print("with crinkly bindlewurdles,",0,150,1,GREEN);
        lcd.Print("Or I will rend thee",0,160,1,GREEN);
        lcd.Print("in the gobberwarts",0,170,1,GREEN);
        lcd.Print("with my blurglecruncheon,",0,180,1,GREEN);
        lcd.Print("see if I don't!",0,190,1,GREEN);
    return micros() - start;
    }

    long testText_bg() {
        long start = micros();
        lcd.Print("Hello World!",RIGHT,0,1,WHITE,BLUE,1);
        lcd.Print(1234.56,RIGHT,10,2,YELLOW,BLUE,1);
        lcd.Print("lcd is ILI9341",RIGHT,30,2,RED,BLUE,1,16);
        lcd.Print("Groop",RIGHT,60,5,GREEN,BLUE,1);
        lcd.Print("I implore thee,",RIGHT,110,2,GREEN,BLUE,1);
        lcd.Print("my foonting turlingdromes.",RIGHT,130,1,GREEN,BLUE,1);
        lcd.Print("And hooptiously drangle me",RIGHT,140,1,GREEN,BLUE,1);
        lcd.Print("with crinkly bindlewurdles,",RIGHT,150,1,GREEN,BLUE,1);
        lcd.Print("Or I will rend thee",RIGHT,160,1,GREEN,BLUE,1);
        lcd.Print("in the gobberwarts",RIGHT,170,1,GREEN,BLUE,1);
        lcd.Print("with my blurglecruncheon,",RIGHT,180,1,GREEN,BLUE,1);
        lcd.Print("see if I don't!",RIGHT,190,1,GREEN,BLUE,1);
    return micros() - start;
    }

    long testLines(uint16_t color) {
        long start, t;
        int x1, y1, x2, y2, w = lcd.Width, h = lcd.Height;

        lcd.Cls(BLACK);
        x1 = y1 = 0;
        y2 = h - 1;
        start = micros();
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, RED);
        x2    = w - 1;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        t     = micros() - start; // fillScreen doesn't count against timing
        delay(200);
        lcd.Fill_Screen(BLACK);
        x1    = w - 1;
        y1    = 0;
        y2    = h - 1;
        start = micros();
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, BLUE);
        x2    = 0;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        t    += micros() - start;
        delay(200);
        lcd.Fill_Screen(BLACK);
        x1    = 0;
        y1    = h - 1;
        y2    = 0;
        start = micros();
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, GREEN);
        x2    = w - 1;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        t    += micros() - start;
        delay(200);
        lcd.Fill_Screen(BLACK);
        x1    = w - 1;
        y1    = h - 1;
        y2    = 0;
        start = micros();
        for(x2=0; x2<w; x2+=6) lcd.Line(x1, y1, x2, y2, YELLOW);
        x2    = 0;
        for(y2=0; y2<h; y2+=6) lcd.Line(x1, y1, x2, y2, color);
        return micros() - start;
    }

    long testFastLines(uint16_t color1, uint16_t color2) {
        int x, y, w = lcd.Width-1, h = lcd.Height-1;
        lcd.Cls(BLACK);
        long start = micros();
        for(y=0; y<h; y+=5) lcd.Fast_HLine(0, y, w, color1);
        for(x=0; x<w; x+=5) lcd.Fast_VLine(x, 0, h, color2);
        return micros() - start;
    }

    long testRects(uint16_t color) {
        int cy = lcd.Height - 1, i2 = 1;
        lcd.Cls(BLACK);
        long start = micros();
        for(int i=0; i < cy/2; i++) {
            lcd.Rectangle(i, i, lcd.Width-i2, lcd.Height-i2, color);
            i2 = i2+2;
        }
        return micros() - start;
    }

    long testFilledRects(uint16_t color1, uint16_t color2) {
        int cy = lcd.Height - 1, i2 = 1, i3 = 2, i4 =2;
        lcd.Cls(BLACK);
        long start = micros();
        for(int i=0; i < cy/2; i++) {
            lcd.Fill_Rectangle(i, i, lcd.Width-i2, lcd.Height-i2, RGB(i4,100,i3));
            i2 = i2+2; i3= i3+2; i4=i4+2;
        }
        return micros() - start;
    }
    
    long testFilledCircles(uint8_t radius, uint16_t color) {
        int x, y, w = lcd.Width, h = lcd.Height, r2 = radius * 2;
        long start = micros();
        for(x=radius; x<w; x+=r2) {
            for(y=radius; y<h; y+=r2) {
            lcd.Fill_Circle(x, y, radius, color);
            }
        }
        return micros() - start;
    }

    long testCircles(uint8_t radius, uint16_t color) {
        int x, y, r2 = radius * 2, w = lcd.Width  + radius, h = lcd.Height + radius;
        long start = micros();
        for(x=0; x<w; x+=r2) {
            for(y=0; y<h; y+=r2) {
            lcd.Circle(x, y, radius, color);
            }
        }
        return micros() - start;
    }

    long testTriangles() {
        int n, i, cx = lcd.Width  / 2 - 1, cy = lcd.Height / 2 - 1;
        lcd.Cls(BLACK);
        n = min(cx, cy);
        long start = micros();
        for(i=0; i<n; i+=5) {
            lcd.Triangle(
            cx    , cy - i, // peak
            cx - i, cy + i, // bottom left
            cx + i, cy + i, // bottom right
            RGB(i*10, i*10, i*10));
        }
        return micros() - start;
    }

    long testFilledTriangles() {
        long start, t = 0;
        int i, cx = lcd.Width  / 2 - 1, cy = lcd.Height / 2 - 1;
        lcd.Cls(BLACK);
        start = micros();
        for(i=min(cx,cy); i>10; i-=5) {
            start = micros();
            lcd.Fill_Triangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, RGB(250, i*10, i*10));
            t += micros() - start;
            lcd.Triangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
            RGB(i*10, i*10, 250));
        }
        return t;
    }

    long testRoundRects() {
        int cy = lcd.Height - 1, i2 = 1;
        lcd.Cls(BLACK);
        long start = micros();
        for(int i=0; i < cy/2; i++) {
            lcd.Round_Rectangle(i, i, lcd.Width-i2, lcd.Height-i2, 8, RED);
            i2 = i2+2;
        }
        return micros() - start;
    }

     long testFilledRoundRects() {
        int cy = lcd.Height - 1, i2 = 1, i3 = 2, i4 =255;
        lcd.Cls(BLACK);
        long start = micros();
        for(int i=0; i < cy/2; i++) {
            lcd.Fill_Round_Rectangle(i, i, lcd.Width-i2, lcd.Height-i2, 8, RGB(0,i4,i3));
            i2 = i2+2; i3= i3+2; i4=i4-2;
        }
        return micros() - start;
    }

    void test() {
 
        Serial.println("ILI9341 Test!"); 
        uint16_t x = lcd.Read_Reg(0xD3,1);
        Serial.print("Display ID: 0x"); Serial.println(x, HEX);
        lcd.Cls(BLACK);

        int d =700;    
        Serial.println(F("Benchmark                Time (microseconds)"));
 
        Serial.print(F("Screen fill              "));
        Serial.println(testFillScreen());
        delay(d);
   
        Serial.print(F("Text fr                  "));
        Serial.println(testText_fr());
        delay(d);

        Serial.print(F("Text bg                  "));
        Serial.println(testText_bg());
        delay(d);

        Serial.print(F("Lines                    "));
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
        delay(d);

        Serial.println(F("Done!"));
        /*for(uint8_t rotation=0; rotation<4; rotation++) {
            lcd.Set_Rotation(rotation);
            testText_fr();
            delay(1000);
            lcd.Fill_Screen(BLACK);
            testText_bg();
            delay(1000);
        }*/
    }
};

Test t1;
void test1() { t1.test();}