#include <LCD_SRW.h> 

uint32_t testFillScreen() {
    uint32_t start = micros();
    lcd.Fill_Screen(BLUE);
    return micros() - start;
}

uint32_t testTextFR() {
    uint32_t start = micros();
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
    //lcd.Print("see if I don't!", CENTER, 238, 1, GREEN,BLUE,0); 
    return micros() - start;
}

uint32_t testTextBG() {
    uint32_t start = micros();
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
    //lcd.Print("see if I don't!", CENTER, 238, 1, GREEN,BLUE,1); 
    return micros() - start;
}

uint32_t testCrossLine() {
    uint32_t start = micros();
    lcd.Line(0, 0, lcd.Width-1, lcd.Height-1, CYAN);
    return micros() - start;
}

uint32_t testLine() {
    uint16_t x1=0, y1=lcd.Height/2, x2=lcd.Width-1, y2=lcd.Height/2; 
    if(lcd.rotation==PORTRAIT) { x1=lcd.Width/2; y1=0; x2=lcd.Width/2, y2=lcd.Height-1; }
    uint32_t start = micros();
    lcd.Line(x1, y1, x2, y2,YELLOW);
    return micros() - start;
}

uint32_t testFastLine() {
    int x,y,w;
    uint32_t start = micros();
    if(lcd.rotation==PORTRAIT) lcd.Fast_VLine(lcd.Width/2, 0, lcd.Height-1, RED);
        else lcd.Fast_HLine(0, lcd.Height/2, lcd.Width-1, RED);
    return micros() - start;
}

uint32_t testRect() {
    uint32_t start = micros();
    lcd.Rectangle(0, 0, lcd.Width, lcd.Height, YELLOW);
    return micros() - start;
}

uint32_t testFillRect() {
    uint32_t start = micros();
    lcd.Fill_Rectangle(0, 0, lcd.Width, lcd.Height, RED);
    return micros() - start;
}

uint32_t testCircle() {
    uint32_t start = micros();
    lcd.Circle(lcd.Width/2, lcd.Height/2, 119, GREEN);
    return micros() - start;
}

uint32_t testFillCircle() {
    uint32_t start = micros();
    lcd.Fill_Circle(lcd.Width/2, lcd.Height/2, 119, RED);
    return micros() - start;
}

uint32_t testTriangle() {
    uint32_t start = micros();
    lcd.Triangle(lcd.Width/2, 0, 0, lcd.Height-1, lcd.Width-1, lcd.Height-1, YELLOW);
    return micros() - start;
}

uint32_t testFillTriangle() {
    uint32_t start = micros();
    lcd.Fill_Triangle(lcd.Width/2, 0, 0, lcd.Height-1, lcd.Width-1, lcd.Height-1, YELLOW);
    return micros() - start;
}

uint32_t testRoundRect() {
    uint32_t start = micros();
    lcd.Round_Rectangle(0, 0, lcd.Width-1,lcd.Height-1, 4, PINK);
    return micros() - start;
}

uint32_t testFillRoundRect() {
    uint32_t start = micros();
    lcd.Fill_Round_Rectangle(0, 0, lcd.Width-1,lcd.Height-1, 4, GREEN);
    return micros() - start;
}

uint32_t dead() { lcd.Fill_Screen(BLACK); return 0; }

static uint32_t (*test_function[15])(void) = {
    dead,
    testFillScreen,
    testTextFR,
    testTextBG,
    testCrossLine,
    testLine,
    testFastLine,
    testRect,
    testFillRect,
    testCircle,
    testFillCircle,
    testTriangle,
    testFillTriangle,
    testRoundRect,
    testFillRoundRect,
};

static uint8_t *test_str[15]= {
    (uint8_t *)"--------------------------  ",
    (uint8_t *)"Full Screen(240x320         ", 
    (uint8_t *)"Text Fore Color             ",
    (uint8_t *)"Text Back Color             ",
    (uint8_t *)"Cross Line(400 pixel)       ",
    (uint8_t *)"Line(320 pixel)             ",
    (uint8_t *)"Fast line(320 pixel)        ",
    (uint8_t *)"Rectangle(w*h)              ",
    (uint8_t *)"Fill Rectangle(w*h)         ",
    (uint8_t *)"Circle(R=240)               ",
    (uint8_t *)"Fill Circle(R=240)          ",
    (uint8_t *)"Triangle(320*288*288)       ",
    (uint8_t *)"Fill Triangle(320*288*288)  ",
    (uint8_t *)"Round Rectangle(w*h)        ",
    (uint8_t *)"Fill Round Rectangle(w*h)   ",
};

uint32_t test_lcd(void){
    lcd.Set_Rotation(LANDSCAPE);
    uint32_t i=0, cycle=15;
    uint32_t buf[15];
    Serial.println();
    Serial.println(F("Test Starting."));
    for(i = 0; i< cycle;i++) {
        buf[i] = test_function[i](); 
        Serial.print(F(test_str[i]));
        Serial.println(buf[i]);
        delay(700); 
        lcd.Fill_Screen(BLACK); 
    }
    Serial.println(F("--------------------------  "));
    Serial.println(F("Done!"));
    for(i = 1; i<cycle; i++) {
        lcd.Print(test_str[i],(lcd.Width-220)/2-1, (lcd.Height-150)/2+i*10-1, 1, ORANGE);
        lcd.Print(buf[i], (lcd.Width-220)/2-1+200, (lcd.Height-150)/2+i*10-1, 1, GREEN);
    }
    delay(2000);
    return 0;
}
    