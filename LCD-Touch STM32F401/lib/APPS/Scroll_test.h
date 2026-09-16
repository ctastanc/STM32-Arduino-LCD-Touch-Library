/*
 * 8bit TFT Library for Arduino_Core_STM32
 *
 * based on MCUFRIEND_kbv.cpp by David Prentice
 * https://github.com/prenticedavid/MCUFRIEND_kbv
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LCD_KBV.h> 
#include <icons.h>

void testText1() {
    lcd.Fill_Screen(BLACK);
    lcd.Print("Hello World!",5,0,1,WHITE);
    lcd.Print(1234.56,5,10,2,YELLOW);
    lcd.Print(3735928559,5,30,3,RED,0,0,16);
    lcd.Print("Groop",5,60,5,GREEN);
    lcd.Print("I implore thee,",5,113,2,GREEN);
    lcd.Print("my foonting turlingdromes.",5,130,1,GREEN);
    lcd.Print("And hooptiously drangle me",5,140,1,GREEN);
    lcd.Print("with crinkly bindlewurdles,",5,150,1,GREEN);
    lcd.Print("Or I will rend thee",5,160,1,GREEN);
    lcd.Print("in the gobberwarts",5,170,1,GREEN);
    lcd.Print("with my blurglecruncheon,",5,180,1,GREEN);
    lcd.Print("see if I don't!",5,190,1,GREEN);
}

void printmsg(int row, const char *msg)
{
    static char primary_msg[64];
    
    if (strlen(primary_msg)) {
      lcd.Print(primary_msg,5,row,1,BLACK,BLACK);
    }
    lcd.Print(msg,5,row,1,YELLOW,BLACK);
    strcpy(primary_msg,msg);
}

void Scroll_Test(void) {
    while (1)
    {
        
        uint8_t aspect;
        uint16_t pixel;
        const char *aspectname[] = {
            "PORTRAIT", "LANDSCAPE", "PORTRAIT_REV", "LANDSCAPE_REV"
        };
        const char *colorname[] = { "BLUE", "GREEN", "RED", "GRAY" };
        uint16_t colormask[] = { 0x001F, 0x07E0, 0xF800, 0xFFFF };
        uint16_t dx, rgb, n, wid, ht, msgline;
        lcd.Set_Rotation(0);
        if (lcd.Height > 64) {
            for (uint8_t cnt = 0; cnt < 4; cnt++) {
                aspect = (cnt + 0) & 3;
                lcd.Set_Rotation(aspect);
                wid = lcd.Width;
                ht = lcd.Height;
                Serial.println("aspect=" + String(aspect));
                Serial.println("width=" + String(wid) + " height=" + String(ht));
                msgline = (ht > 160) ? 200 : 112;
                testText1();
                
                // Show COLOR GRADES
                dx = wid / 32;
                for (n = 0; n < 32; n++) {
                    rgb = n * 8;
                    rgb = RGB(rgb, rgb, rgb).val;
                    lcd.Fill_Rectangle_WH(n * dx, 48, dx, 63, rgb & colormask[aspect]);
                }
                lcd.Print(colorname[aspect],5,72,2,colormask[aspect],BLACK);

                lcd.Print(" COLOR GRADES",65,72,2,WHITE);

                printmsg(220, aspectname[aspect]);
                delay(1000);
                lcd.Pixel(0, 0, YELLOW);
                pixel = lcd.Read_Pixel(0, 0);

                lcd.Set_Text_Size((ht > 160) ? 2 : 1); //for messages

                // Show penguin icon
                extern const uint8_t penguin[];
                CS_L; lcd.Set_Addr_Window(wid - 40 - 40, 20 + 0, wid - 1 - 40, 20 + 39);
                lcd.Push_Any_Color((uint8_t *)penguin, 1600, 1,1);

                // Scroll Screen
                //CS_L; lcd.Set_Addr_Window(0, 0, wid - 1, ht - 1);
                //if (aspect & 1) lcd.Rectangle_WH(wid - 1, 0, 1, ht, WHITE);
                //else lcd.Rectangle_WH(0, ht - 1, wid, 1, WHITE);
                printmsg(msgline, "VERTICAL SCROLL UP");
                uint16_t maxscroll;
                if (lcd.Get_Rotation() & 1) maxscroll = wid;
                else maxscroll = ht;
                int step = -1;
                if ( wid < ht ) step = 1;

                // Scroll Up
                for (int16_t i = 1; i <= maxscroll; i++) {
                    lcd.Vert_Scroll(0, maxscroll, i * step);
                    delay(10);
                }
                delay(1000);

                // Scroll Down
                printmsg(msgline, "VERTICAL SCROLL DN");
                for (int16_t i = 1; i <= maxscroll; i++) {
                    lcd.Vert_Scroll(0, maxscroll, (0 - i) * step);
                    delay(10);
                }
                    lcd.Vert_Scroll(0, maxscroll, 0);
                printmsg(msgline, "SCROLL DISABLED");

                // Scroll COLOR GRADES
                delay(1000);
                if ((aspect & 1) == 0) { //Portrait
                    int stline;
                    stline = 128;
                    step = -1;
                    if (wid < ht) {
                    stline = 48;
                    step = 1;
                    }
                    //lcd.Set_Text_Color(BLUE, BLACK);
                    printmsg(msgline, "ONLY THE COLOR BAND");
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.Vert_Scroll(stline, 64, i * step);
                        delay(20);
                    }
                    delay(1000);
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.Vert_Scroll(stline, 64, (0 - i) * step);
                        delay(20);
                    }
                    delay(1000);

    #if 0
                    if ( wid < ht ) {
                    stline = 48;
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.vertScroll(stline, 64, i);
                        delay(20);
                    }
                    delay(1000);
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.vertScroll(stline, 64, 0 - i);
                        delay(20);
                    }
                    delay(1000);
                    } else {
                    stline = 128;
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.vertScroll(stline, 64, 0 - i);
                        delay(20);
                    }
                    delay(1000);
                    for (int16_t i = 1; i <= 64; i++) {
                        lcd.vertScroll(stline, 64, i);
                        delay(20);
                    }
                    delay(1000);
                    }
    #endif                  
                }

                printmsg(msgline, "INVERT DISPLAY");
                lcd.Invert_Display(true);
                delay(3000);
                printmsg(msgline, "NORMAL DISPLAY");
                lcd.Invert_Display(false);
                delay(3000);
            }
        }
    }
}

