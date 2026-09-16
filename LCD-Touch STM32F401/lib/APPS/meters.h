/*
 * 8bit TFT Library for Arduino_Core_STM32
 *
 * Demo based on:
 * TFT_Meters by Bodmar
 * web: https://github.com/Bodmer/TFT_eSPI/tree/master/examples/320%20x%20240/TFT_Meters
 *
 */


#include <LCD_KBV.h>

// If you want to change the default serial instance pin, set it to 1.

#define LOOP_PERIOD 35 // Display updates every 35 ms
class meter{
    public:
    
    float ltx = 0;    // Saved x coord of bottom of needle
    uint16_t osx = 120, osy = 120; // Saved x & y coords
    uint32_t updateTime = 0;       // time for next update
    int16_t  LinearY = 150;

    int old_analog =  -999; // Value last displayed
    int old_digital = -999; // Value last displayed

    int value[6] = {0, 0, 0, 0, 0, 0};
    int old_value[6] = { -1, -1, -1, -1, -1, -1};
    int d = 0;

    // #########################################################################
    // Update needle position
    // This function is blocking while needle moves, time depends on ms_delay
    // 10ms minimises needle flicker if text is drawn within needle sweep area
    // Smaller values OK if text not in sweep area, zero for instant movement but
    // does not look realistic... (note: 100 increments for full scale deflection)
    // #########################################################################
    void plotNeedle(int value, byte ms_delay)
    {
    //lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    char buf[8]; dtostrf(value, 3, 0, buf);
    lcd.Fill_Rectangle_WH(10,99,33,20,WHITE);
    lcd.Print(buf, 10, 119 - 20, 2, BLACK, WHITE);
    //lcd.drawRightString(buf, 40, 119 - 20, 2);

    if (value < -10) value = -10; // Limit value to emulate needle end stops
    if (value > 110) value = 110;

    // Move the needle util new value reached
    while (!(value == old_analog)) {
        if (old_analog < value) old_analog++;
        else old_analog--;

        if (ms_delay == 0) old_analog = value; // Update immediately id delay is 0

        float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
        // Calcualte tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        lcd.Line(120 + 20 * ltx - 1, 140 - 20, osx - 1, osy, WHITE);
        lcd.Line(120 + 20 * ltx, 140 - 20, osx, osy, WHITE);
        lcd.Line(120 + 20 * ltx + 1, 140 - 20, osx + 1, osy, WHITE);

        // Re-plot text under needle
        //lcd.setTextColor(TFT_BLACK);
    
        lcd.Print("%RH", 80, 100, 1,BLACK); // // Comment out to avoid font 4

        //lcd.drawCentreString("%RH", 120, 70, 4); // // Comment out to avoid font 4

        // Store new needle end coords for next erase
        ltx = tx;
        osx = sx * 98 + 120;
        osy = sy * 98 + 140;

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        lcd.Line(120 + 20 * ltx - 1, 140 - 20, osx - 1, osy, RED);
        lcd.Line(120 + 20 * ltx, 140 - 20, osx, osy, MAGENTA);
        lcd.Line(120 + 20 * ltx + 1, 140 - 20, osx + 1, osy, RED);

        // Slow needle down slightly as it approaches new postion
        if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;

        // Wait before next update
        delay(ms_delay);
    }
    }

    // #########################################################################
    //  Draw the analogue meter on the screen
    // #########################################################################
    void analogMeter()
    {
    // Meter outline
    lcd.Fill_Rectangle_WH(0, 0, 239, 126, LIGHTGREY);
    lcd.Fill_Rectangle_WH(5, 3, 230, 119, WHITE);

    //lcd.setTextColor(BLACK);  // Text colour

    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {
        // Long scale tick length
        int tl = 15;

        // Coodinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (100 + tl) + 120;
        uint16_t y0 = sy * (100 + tl) + 140;
        uint16_t x1 = sx * 100 + 120;
        uint16_t y1 = sy * 100 + 140;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (100 + tl) + 120;
        int y2 = sy2 * (100 + tl) + 140;
        int x3 = sx2 * 100 + 120;
        int y3 = sy2 * 100 + 140;

        // Yellow zone limits
        //if (i >= -50 && i < 0) {
        //  lcd.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
        //  lcd.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
        //}

        // Green zone limits
        if (i >= 0 && i < 25) {
        lcd.Fill_Triangle(x0, y0, x1, y1, x2, y2, GREEN);
        lcd.Fill_Triangle(x1, y1, x2, y2, x3, y3, GREEN);
        }

        // Orange zone limits
        if (i >= 25 && i < 50) {
        lcd.Fill_Triangle(x0, y0, x1, y1, x2, y2, ORANGE);
        lcd.Fill_Triangle(x1, y1, x2, y2, x3, y3, ORANGE);
        }

        // Short scale tick length
        if (i % 25 != 0) tl = 8;

        // Recalculate coords incase tick lenght changed
        x0 = sx * (100 + tl) + 120;
        y0 = sy * (100 + tl) + 140;
        x1 = sx * 100 + 120;
        y1 = sy * 100 + 140;

        // Draw tick
        lcd.Line(x0, y0, x1, y1, BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
        // Calculate label positions
        x0 = sx * (100 + tl + 10) + 120;
        y0 = sy * (100 + tl + 10) + 140;
        switch (i / 25) {
    #if 0
            case -2: lcd.drawCentreString("0", x0, y0 - 12, 2); break;
            case -1: lcd.drawCentreString("25", x0, y0 - 9, 2); break;
            case 0: lcd.drawCentreString("50", x0, y0 - 6, 2); break;
            case 1: lcd.drawCentreString("75", x0, y0 - 9, 2); break;
            case 2: lcd.drawCentreString("100", x0, y0 - 12, 2); break;
    #endif
            case -2: lcd.Print("0", x0-10, y0 - 12, 2, BLACK); break;
            case -1: lcd.Print("25", x0-10, y0 - 9, 2,BLACK); break;
            case 0: lcd.Print("50", x0-10, y0 - 6, 2,BLACK); break;
            case 1: lcd.Print("75", x0-10, y0 - 9, 2,BLACK); break;
            case 2: lcd.Print("100", x0-15, y0 - 12, 2,BLACK); break;
        }
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * 100 + 120;
        y0 = sy * 100 + 140;
        // Draw scale arc, don't draw the last part
        if (i < 50) lcd.Line(x0, y0, x1, y1, BLACK);
    }

    lcd.Print("%RH", 5 + 230 - 40, 119 - 20, 2, BLACK); // Units at bottom right
    //lcd.drawString("%RH", 5 + 230 - 40, 119 - 20, 2); // Units at bottom right
    //lcd.drawCentreString("%RH", 120, 70, 4); // Comment out to avoid font 4
    lcd.Rectangle_WH(5, 3, 230, 119, BLACK); // Draw bezel line

    plotNeedle(0, 0); // Put meter needle at 0
    }


    // #########################################################################
    //  Draw a linear meter on the screen
    // #########################################################################
    void plotLinear(const char *label, int x, int y)
    {
    int w = 36;
    lcd.Rectangle_WH(x, y, w, 155, LIGHTGREY);
    lcd.Rectangle_WH(x + 2, y + 19, w - 3, 155 - 38, WHITE);
    //lcd.setTextColor(CYAN, BLACK);
    lcd.Print(label, (x + w / 2) - 10, y + 2, 2, CYAN, BLACK);
    //lcd.drawCentreString(label, x + w / 2, y + 2, 2);

    for (int i = 0; i < 110; i += 10)
    {
        lcd.Fast_HLine(x + 20, y + 27 + i, 6, BLACK);
    }

    for (int i = 0; i < 110; i += 50)
    {
        lcd.Fast_HLine(x + 20, y + 27 + i, 9, BLACK);
    }

    lcd.Fill_Triangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 - 5, RED);
    lcd.Fill_Triangle(x + 3, y + 127, x + 3 + 16, y + 127, x + 3, y + 127 + 5, RED);

    //_drawString("---", (x + w / 2) - 18, y + 155 - 18, 2);
    //lcd.drawCentreString("---", x + w / 2, y + 155 - 18, 2);
    }

    // #########################################################################
    //  Adjust 6 linear meter pointer positions
    // #########################################################################
    void plotPointer(int y)
    {
    //  int dy0 = 187;
    int dy0 = y + 27;
    byte pw = 16;

    // lcd.setTextColor(TFT_GREEN, TFT_BLACK);

    // Move the 6 pointers one pixel towards new value
    for (int i = 0; i < 6; i++)
    {
        char buf[8]; dtostrf(value[i], 3, 0, buf);
    //    _drawString(buf, (i * 40 + 36 - 5) - 50, 187 - 27 + 155 - 18, 2);
        lcd.Print(buf, (i * 40 + 36 - 5) - 35, y + 137, 2, GREEN, BLACK);
        //lcd.drawRightString(buf, i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);

        int dx = 3 + 40 * i;
        if (value[i] < 0) value[i] = 0; // Limit value to emulate needle end stops
        if (value[i] > 100) value[i] = 100;

        int dy;
        while (!(value[i] == old_value[i])) {
        //dy = 187 + 100 - old_value[i];
        dy = dy0 + 100 - old_value[i];
        if (old_value[i] > value[i])
        {
            lcd.Line(dx, dy - 5, dx + pw, dy, WHITE);
            old_value[i]--;
            lcd.Line(dx, dy + 6, dx + pw, dy + 1, RED);
        }
        else
        {
            lcd.Line(dx, dy + 5, dx + pw, dy, WHITE);
            old_value[i]++;
            lcd.Line(dx, dy - 6, dx + pw, dy - 1, RED);
        }
        }
    }
    }

    void meters() {
        lcd.Set_Rotation(0);
        lcd.Fill_Screen(BLACK);
        analogMeter(); // Draw analogue meter
        // Draw 6 linear meters
        byte d = 40;
        plotLinear("A0", 0, LinearY);
        plotLinear("A1", 1 * d, LinearY);
        plotLinear("A2", 2 * d, LinearY);
        plotLinear("A3", 3 * d, LinearY);
        plotLinear("A4", 4 * d, LinearY);
        plotLinear("A5", 5 * d, LinearY);
        updateTime = millis(); // Next update time
        while(1){
            if (updateTime <= millis()) {
                updateTime = millis() + LOOP_PERIOD;

                d += 4; if (d >= 360) d = 0;

                //value[0] = map(analogRead(A0), 0, 1023, 0, 100); // Test with value form Analogue 0

                // Create a Sine wave for testing
                value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);
                value[1] = 50 + 50 * sin((d + 60) * 0.0174532925);
                value[2] = 50 + 50 * sin((d + 120) * 0.0174532925);
                value[3] = 50 + 50 * sin((d + 180) * 0.0174532925);
                value[4] = 50 + 50 * sin((d + 240) * 0.0174532925);
                value[5] = 50 + 50 * sin((d + 300) * 0.0174532925);

                //unsigned long t = millis();

                plotPointer(LinearY);

                plotNeedle(value[0], 0);

                //Serial.println(millis()-t); // Print time taken for meter update
            }
        }
    }
};

meter mt;