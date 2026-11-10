#include <LCD_SRW.h> 

uint32_t testFillScreen() {
    uint32_t start = micros();
    lcd.Fill_Screen(BLUE);
    uint32_t elapsed_us = micros() - start;
    lcd.PrintFont(font19,(String)("Frequency : "+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,WHITE,BLUE,0);
    lcd.PrintFont(font19,(String)("PCLK1 : "+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,100,3,WHITE);
    lcd.PrintFont(font19,(String)("PCLK2 : "+String(HAL_RCC_GetPCLK2Freq()/1000000) +" Mhz"),CENTER,145,3,WHITE);
    lcd.Fill_Rectangle(65,177,188,23,BLACK);
    lcd.PrintFont(font19,(String)("Full Screen : "+String(elapsed_us)+" us"),CENTER,180,3,RED);
    return elapsed_us;
}

uint32_t testTextBG() {
    uint32_t start = micros();
    lcd.PrintFont(font9,"Hello world!", LEFT, 24, 1, WHITE,BLUE,1);
    lcd.PrintFont(font19,1234.56, LEFT, 35, 2, YELLOW,BLUE,1);
    lcd.PrintFont(font19,"Lcd ILI9341", LEFT, 57, 2, RED,BLUE,1);
    lcd.PrintFont(font30,"ENERGY", LEFT, 85, 4, GREEN,BLUE,1);
    lcd.PrintFont(font19,"In Universe.", LEFT, 120, 2, GREEN,BLUE,1);
    lcd.PrintFont(font9,"What will happen in the", LEFT, 145, 1, RED,BLUE,1);
    lcd.PrintFont(font9,"future is also one of the", LEFT, 156, 1, WHITE,BLUE,1);
    lcd.PrintFont(font9,"mysteries of the universe.", LEFT, 167, 1, GREEN,BLUE,1);
    lcd.PrintFont(font9,"If you aspire to unravel", LEFT, 178, 1, GREEN,BLUE,1);
    lcd.PrintFont(font9,"the secrets of the universe,", LEFT, 189, 1, GREEN,BLUE,1);
    lcd.PrintFont(font9,"think in terms of energy,", LEFT, 200, 1, GREEN,BLUE,1);
    lcd.PrintFont(font9,"frequency, and vibrations.", LEFT, 211, 1, GREEN,BLUE,1);
    return micros() - start;
}

uint32_t testTextFR() {
    uint32_t start = micros();
    lcd.PrintFont(font9,"Hello world!", RIGHT, 24, 1, YELLOW,BLUE,0);
    lcd.PrintFont(font19,1234.56, RIGHT, 35, 2, YELLOW,BLUE,0);
    lcd.PrintFont(font19,"Lcd ILI9341", RIGHT, 57, 2, RED,BLUE,0);
    lcd.PrintFont(font30,"ENERGY", RIGHT, 85, 4, GREEN,BLUE,0);
    lcd.PrintFont(font19,"In Universe.", RIGHT, 120, 2, GREEN,BLUE,0);
    lcd.PrintFont(font9,"What will happen in the", RIGHT, 145, 1, RED,BLUE,0);
    lcd.PrintFont(font9,"future is also one of the", RIGHT, 156, 1, WHITE,BLUE,0);
    lcd.PrintFont(font9,"mysteries of the universe.", RIGHT, 167, 1, GREEN,BLUE,0);
    lcd.PrintFont(font9,"If you aspire to unravel", RIGHT, 178, 1, GREEN,BLUE,0);
    lcd.PrintFont(font9,"the secrets of the universe,", RIGHT, 189, 1, GREEN,BLUE,0);
    lcd.PrintFont(font9,"think in terms of energy,", RIGHT, 200, 1, GREEN,BLUE,0);
    lcd.PrintFont(font9,"frequency, and vibrations.", RIGHT, 211, 1, GREEN,BLUE,0);
    //lcd.PrintFont(font45,"WMQRT", CENTER, 138, 4, GREEN,BLUE,0);
    return micros() - start;
}

uint32_t testCrossLine() {
    uint32_t start = micros();
    lcd.Line(0, 0, lcd.Width-1, lcd.Height-1, CYAN);
    return micros() - start;
}

uint32_t testLine() {
    uint16_t x1=0, y1=lcd.Height/2, x2=lcd.Width-1, y2=lcd.Height/2; 
    //if(lcd.rotation==PORTRAIT) { x1=lcd.Width/2; y1=0; x2=lcd.Width/2, y2=lcd.Height-1; }
    uint32_t start = micros();
    lcd.Line(x1, y1, x2, y2,YELLOW);
    return micros() - start;
}

uint32_t testFastLine() {
    int x,y,w;
    uint32_t start = micros();
    //if(lcd.rotation==PORTRAIT) lcd.Fast_VLine(lcd.Width/2, 0, lcd.Height-1, RED);
       // else 
    lcd.Fast_HLine(0, lcd.Height/2, lcd.Width-1, RED);
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

uint32_t dead() { lcd.Fill_Screen(CYAN); return 0; }

static uint32_t (*test_function[15])(void) = {
    dead,
    testFillScreen,
    testTextBG,
    testTextFR,
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

static const char *test_str[15]= {
    "--------------------------\t",
    "Full Screen(240x320) \t\t", 
    "Text Back Color \t\t",
    "Text Fore Color \t\t",
    "Cross Line(400 pixel) \t\t",
    "Line(320 pixel) \t\t",
    "Fast line(320 pixel) \t\t",
    "Rectangle(w*h) \t\t\t",
    "Fill Rectangle(w*h) \t\t",
    "Circle(R=240) \t\t\t",
    "Fill Circle(R=240) \t\t",
    "Triangle(320*288*288) \t\t",
    "Fill Triangle(320*288*288) \t",
    "Round Rectangle(w*h) \t\t",
    "Fill Round Rectangle(w*h) \t",
};

uint32_t test_lcd(void){
    lcd.Set_Rotation(LANDSCAPE);
    uint32_t i=0, cycle=15;
    uint32_t buf[15];
    Serial.println();
    Serial.println(F("Test Starting."));
    for(i ; i< cycle;i++) {
        buf[i] = test_function[i](); 
        Serial.print(F(test_str[i])); //Serial.print(F("\t\t"));
        Serial.println(buf[i]);
        delay(1000); 
        if (i != 2 /*&& i !=3*/ ) lcd.Cls(BLACK); 
    }
    Serial.println(F("--------------------------  "));
    Serial.println(F("Done!"));
    for(i = 1; i<cycle; i++) {
        lcd.PrintFont(font9,test_str[i],(lcd.Width-220)/2-1, (lcd.Height-150)/2+i*10-1, 1, ORANGE);
        lcd.PrintFont(font9,buf[i], (lcd.Width-220)/2-1+200, (lcd.Height-150)/2+i*10-1, 1, GREEN);
    }
    delay(2000);
    return 0;
}
    