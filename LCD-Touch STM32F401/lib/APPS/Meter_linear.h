/*
 * 8bit TFT Library for Arduino_Core_STM32
 *
 * Demo based on:
 * TFT_Meter_linear by Bodmar
 * web: https://github.com/Bodmer/TFT_eSPI/tree/master/examples/320%20x%20240/TFT_Meter_linear
 *
 */

// Define meter size as 1 for lcd.rotation(0) or 1.3333 for lcd.rotation(1)
#define M_SIZE 1.3333

#include <LCD_KBV.h>

// If you want to change the default serial instance pin, set it to 1.
class metlin{
    public:
    float ltx = 0;    // Saved x coord of bottom of needle
    uint16_t osx = M_SIZE*120, osy = M_SIZE*120; // Saved x & y coords
    uint32_t updateTime = 0;       // time for next update

    int old_analog =  -999; // Value last displayed

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
    char buf[8]; dtostrf(value, 4, 0, buf);
    lcd.Print("%RH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 2,BLACK,WHITE); // Units at bottom right
    //lcd.drawString("%RH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 2,BLACK,WHITE); // Units at bottom right
    lcd.Fill_Rectangle_WH(M_SIZE*40-40, M_SIZE*(119 - 20),33,16,WHITE);
    lcd.Print(buf, M_SIZE*40-40, M_SIZE*(119 - 20), 2,BLACK,WHITE,1);
    //lcd.drawRightString(buf, M_SIZE*40, M_SIZE*(119 - 20), 2);

    if (value < -10) value = -10; // Limit value to emulate needle end stops
    if (value > 110) value = 110;

    // Move the needle until new value reached
    while (!(value == old_analog)) {
        if (old_analog < value) old_analog++;
        else old_analog--;

        if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

        float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
        // Calcualte tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        lcd.Line(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, WHITE);
        lcd.Line(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, WHITE);
        lcd.Line(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, WHITE);

        // Re-plot text under needle
        //lcd.setTextColor(TFT_BLACK);
        //lcd.setFont(&FreeMonoBold24pt7b);
        lcd.Print("%RH", M_SIZE*80, M_SIZE*100, 2,BLACK); // // Comment out to avoid font 4
        //lcd.setFont();
        //lcd.drawCentreString("%RH", M_SIZE*120, M_SIZE*70, 4); // // Comment out to avoid font 4

        // Store new needle end coords for next erase
        ltx = tx;
        osx = M_SIZE*(sx * 98 + 120);
        osy = M_SIZE*(sy * 98 + 140);

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        lcd.Line(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, RED);
        lcd.Line(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, MAGENTA);
        lcd.Line(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, RED);

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
    lcd.Fill_Rectangle_WH(0, 0, M_SIZE*239, M_SIZE*126, LIGHTGREY);
    lcd.Fill_Rectangle_WH(5, 3, M_SIZE*230, M_SIZE*119, WHITE);

    //lcd.setTextColor(TFT_BLACK);  // Text colour

    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {
        // Long scale tick length
        int tl = 15;

        // Coodinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
        uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
        uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
        uint16_t y1 = sy * M_SIZE*100 + M_SIZE*140;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
        int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*140;
        int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
        int y3 = sy2 * M_SIZE*100 + M_SIZE*140;

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
        x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
        y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
        x1 = sx * M_SIZE*100 + M_SIZE*120;
        y1 = sy * M_SIZE*100 + M_SIZE*140;

        // Draw tick
        lcd.Line(x0, y0, x1, y1, BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
        // Calculate label positions
        x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
        y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*140;
        switch (i / 25) {
    #if 0
            case -2: lcd.drawCentreString("0", x0, y0 - 12, 2); break;
            case -1: lcd.drawCentreString("25", x0, y0 - 9, 2); break;
            case 0: lcd.drawCentreString("50", x0, y0 - 7, 2); break;
            case 1: lcd.drawCentreString("75", x0, y0 - 9, 2); break;
            case 2: lcd.drawCentreString("100", x0, y0 - 12, 2); break;
    #endif
            case -2: lcd.Print("0", x0-10, y0 - 12, 2,BLACK); break;
            case -1: lcd.Print("25", x0-10, y0 - 9, 2,BLACK); break;
            case 0: lcd.Print("50", x0-10, y0 - 7, 2,BLACK); break;
            case 1: lcd.Print("75", x0-10, y0 - 9, 2,BLACK); break;
            case 2: lcd.Print("100", x0-10, y0 - 12, 2,BLACK); break;
        }
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * M_SIZE*100 + M_SIZE*120;
        y0 = sy * M_SIZE*100 + M_SIZE*140;
        // Draw scale arc, don't draw the last part
        if (i < 50) lcd.Line(x0, y0, x1, y1, BLACK);
    }

    lcd.Print("%RH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 2,BLACK); // Units at bottom right
    //lcd.drawString("%RH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 2); // Units at bottom right

    //lcd.drawCentreString("%RH", M_SIZE*120, M_SIZE*70, 4); // Comment out to avoid font 4

    lcd.Rectangle_WH(5, 3, M_SIZE*230, M_SIZE*119, BLACK); // Draw bezel line
    plotNeedle(0, 0); // Put meter needle at 0
    }

    void Meter_Linear() {
    lcd.Set_Rotation(1);
    lcd.Fill_Screen(BLACK);
    analogMeter(); // Draw analogue meter
    updateTime = millis(); // Next update time
    while(1) {
            if (updateTime <= millis()) {
                updateTime = millis() + 35; // Update emter every 35 milliseconds
            
                // Create a Sine wave for testing
                d += 4; if (d >= 360) d = 0;
                value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);
            
                plotNeedle(value[0], 0); // It takes between 2 and 12ms to replot the needle with zero delay
            }
        }
    }
};

metlin ml;