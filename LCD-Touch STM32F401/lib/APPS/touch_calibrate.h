#include <touch_screen.h>

void print_point(int order, int32_t MARGIN_X, int32_t MARGIN_Y) {
    lcd.Fill_Screen(BLACK);
    lcd.Rectangle(0, 0, lcd.Width, lcd.Height, YELLOW);
    if(order==1) {
        lcd.Line(MARGIN_X - 15, MARGIN_Y, MARGIN_X + 15, MARGIN_Y, RED);
        lcd.Line(MARGIN_X, MARGIN_Y - 15, MARGIN_X, MARGIN_Y + 15, RED);
    } else if(order==2) {
        lcd.Line(lcd.Width - MARGIN_X - 15, MARGIN_Y, lcd.Width - MARGIN_X + 15, MARGIN_Y, RED);
        lcd.Line(lcd.Width - MARGIN_X, MARGIN_Y - 15, lcd.Width - MARGIN_X, MARGIN_Y + 15, RED);
    } else if(order==3) {
        lcd.Line(lcd.Width - MARGIN_X - 15, lcd.Height - MARGIN_Y, lcd.Width - MARGIN_X + 15, lcd.Height - MARGIN_Y, RED);
        lcd.Line(lcd.Width - MARGIN_X, lcd.Height - MARGIN_Y - 15, lcd.Width - MARGIN_X, lcd.Height - MARGIN_Y + 15, RED);
    } else if(order==4) {
        lcd.Line(MARGIN_X - 15, lcd.Height - MARGIN_Y, MARGIN_X + 15, lcd.Height - MARGIN_Y, RED);
        lcd.Line(MARGIN_X, lcd.Height - MARGIN_Y - 15, MARGIN_X, lcd.Height - MARGIN_Y + 15, RED);
    }
    lcd.Print((String)(String(order)+". Touch the center."), CENTER, lcd.Height / 4, 2, WHITE);
}

void print_results(int32_t final_minX, int32_t final_maxX, int32_t final_minY, int32_t final_maxY) {
    lcd.Fill_Screen(BLACK);
    lcd.Print("Calibrated !", CENTER, lcd.Height / 4, 2, GREEN);

    lcd.Print((String)("MIN_X: "+String(final_minX)), lcd.Width/2-50, lcd.Height / 2, 2, WHITE);
    lcd.Print((String)("MAX_X: "+String(final_maxX)), lcd.Width/2-50, lcd.Height / 2+20, 2, WHITE);
    lcd.Print((String)("MIN_Y: "+String(final_minY)), lcd.Width/2-50, lcd.Height / 2+40, 2, WHITE);
    lcd.Print((String)("MAX_Y: "+String(final_maxY)), lcd.Width/2-50, lcd.Height / 2+60, 2, WHITE);
}

void touch_calibration() {
    TSPoint p;
    uint16_t samples_X[4] = {0};
    uint16_t samples_Y[4] = {0};
    const int32_t MARGIN_X = 20;
    const int32_t MARGIN_Y = 20;

    for(int i=0; i<4;i++) {
        print_point(i+1, MARGIN_X, MARGIN_Y);
        while(true) { 
            p = ts.getRawPoint(); 
            if (p.v ) { 
                samples_X[i] = p.x; 
                samples_Y[i] = p.y; 
                while(true) { p = ts.getRawPoint(); if(!p.z) break; } 
                break; 
            } 
        }
        delay(400);
    }
    // Filtering out raw min/max limits from the input
    uint16_t inner_minX = samples_X[0], inner_maxX = samples_X[0];
    uint16_t inner_minY = samples_Y[0], inner_maxY = samples_Y[0];
    for(int i = 1; i < 4; i++) {
        if(samples_X[i] < inner_minX) inner_minX = samples_X[i];
        if(samples_X[i] > inner_maxX) inner_maxX = samples_X[i];
        if(samples_Y[i] < inner_minY) inner_minY = samples_Y[i];
        if(samples_Y[i] > inner_maxY) inner_maxY = samples_Y[i];
    }
    // Active pixels according to screen rotation.
    int32_t active_width  = (lcd.rotation % 2 == 0) ? lcd.Width : lcd.Height;
    int32_t active_height = (lcd.rotation % 2 == 0) ? lcd.Height : lcd.Width;

    int32_t inner_rangeX = inner_maxX - inner_minX;
    int32_t inner_rangeY = inner_maxY - inner_minY;
    // Formula for outward stretching relative to the exact pixel position (20, 20) of the cross centers.
    int32_t final_minX = inner_minX - (MARGIN_X * inner_rangeX) / (active_width -  MARGIN_X);
    int32_t final_maxX = inner_maxX + (MARGIN_X * inner_rangeX) / (active_width -  MARGIN_X);
    int32_t final_minY = inner_minY - (MARGIN_Y * inner_rangeY) / (active_height -  MARGIN_Y);
    int32_t final_maxY = inner_maxY + (MARGIN_Y * inner_rangeY) / (active_height -  MARGIN_Y);

    if(final_minX < 0) final_minX = 0; if(final_maxX > RES_VALUE) final_maxX = RES_VALUE;
    if(final_minY < 0) final_minY = 0; if(final_maxY > RES_VALUE) final_maxY = RES_VALUE;

    print_results(final_minX, final_maxX, final_minY, final_maxY);
    
    while(true);
}
