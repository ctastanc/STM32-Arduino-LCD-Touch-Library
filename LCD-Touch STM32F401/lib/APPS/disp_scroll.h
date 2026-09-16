#include <LCD_KBV.h> //Hardware-specific library
#include <icons.h>

    const char *aspect_name[] = {"PORTRAIT", "LANDSCAPE", "PORTRAIT_REV", "LANDSCAPE_REV"};
    const char *color_name[] = { "BLUE", "GREEN", "RED", "WHITE" ,"CYAN","MAGENTA","YELLOW"};
    uint16_t color_mask[] = { 0x001F, 0x07E0, 0xF800, 0xFFFF,0x07FF,0xF81F,0xFFE0 };
   

class disp_s {
    public:
    void show_pic(void)
    {
        int i;
        CS_L; lcd.Set_Addr_Window(lcd.Width-40-40, 20, lcd.Width-40-1, 59);
        lcd.Push_Any_Color((uint8_t *)penguin, 1600, 1, 1);
    }

    void windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf)
    {
        if (dx) { 
            for (int16_t row = 0; row < ht; row++) {
                lcd.Read_GRAM(x, y + row, buf,wid, 1);
                CS_L; lcd.Set_Addr_Window(x, y + row, x + wid - 1, y + row);
                lcd.Push_Any_Color(buf + dx, wid - dx, 1,0);
                lcd.Push_Any_Color(buf + 0, dx, 0,0);
            }
        }
        if (dy) {
            for (int16_t col = 0; col < wid; col++) {
                lcd.Read_GRAM(x + col, y, buf,1, ht);
                CS_L; lcd.Set_Addr_Window(x + col, y, x + col, y + ht - 1);
                lcd.Push_Any_Color(buf + dy, ht - dy, 1,0);
                lcd.Push_Any_Color(buf + 0, dy, 0,0);
            }
        }
    }

    
    void color_test()  {
        int n,i;
        int cnum = sizeof(color_mask)/sizeof(uint16_t);
        for(i=0;i<cnum;i++)
        {
            for(n=0;n<32;n++) {
            lcd.Fill_Rectangle_XY(n*lcd.Width/32,(lcd.Height/cnum)*i,(n+1)*lcd.Width/32,(lcd.Height/cnum)*(i+1),
                                    RGB(n*8, n*8, n*8).val&color_mask[i]);
            }
            lcd.Print(color_name[i],0,((lcd.Height/cnum)-16)/2+(lcd.Height/cnum)*i,2,color_mask[i], BLACK,0);
        }
    }

    void text_test(void)
    {
        lcd.Print("Hello World!",0,0,1,RED, BLACK,1);
        lcd.Print(01234.56789, 0, 8,2,YELLOW); 
        lcd.Print("ABCDEF123456",0,24,3,BLUE, BLACK,1);
        lcd.Print("Good",0,56,5,GREEN, BLACK,1);
        lcd.Print("By utilizing breath",0,96,2,GREEN, BLACK,1);
        lcd.Print("we soften our experiences.",0,112,1,GREEN, BLACK,1);
        lcd.Print("If we dam them up,",0,120,1,GREEN, BLACK,1);
        lcd.Print("our lives will stagnate,",0,128,1,GREEN, BLACK,1);
        lcd.Print("but we keep them flowing,",0,136,1,GREEN, BLACK,1);
        lcd.Print("we allow more newness",0,144,1,GREEN, BLACK,1);
        lcd.Print("and greater experiences",0,152,1,GREEN, BLACK,1);
        lcd.Print("to blossom.Yes OK",0,160,1,GREEN, BLACK,1);
    }

    void disp_scroll() 
    {
        lcd.Fill_Screen(BLACK);
        while(1){
            uint16_t max_scroll,rotation,i,n;
            lcd.Set_Rotation(0);
            uint16_t scrollbuf[lcd.Height]; 
            for(rotation = 0;rotation<4;rotation++) {
                lcd.Set_Rotation(rotation);
                lcd.Fill_Screen(BLACK);
                color_test();
                delay(1000);
                lcd.Fill_Screen(BLACK);
                text_test();    
                delay(1000);
                for(n=0;n<32;n++) {
                    lcd.Fill_Rectangle_XY(n*lcd.Width/32,48,(n+1)*lcd.Width/32,112,
                                        RGB(n*8, n*8, n*8).val&color_mask[rotation]);
                }
                lcd.Print("COLOR TESTING",50,72,2,color_mask[rotation+1], BLACK,0);
                lcd.Print(aspect_name[rotation],0,184,2,WHITE, BLACK,1);       
                show_pic();
                lcd.Print("VERTICAL SCROLL UP",0,168,2,YELLOW, BLACK,1);  
                if(lcd.Get_Rotation()&1) {
                    max_scroll = lcd.Width;
                }
                else {
                    max_scroll = lcd.Height;
                }
                for(i=1;i<=max_scroll;i++) {
                    lcd.Vert_Scroll(0, max_scroll, i);
                    delay(5);
                }
                lcd.Vert_Scroll(0, max_scroll, 0);
                lcd.Print("VERTICAL SCROLL DN",0,168,2,GREEN, BLACK,1); 
                for(i=1;i<=max_scroll;i++) {
                    lcd.Vert_Scroll(0, max_scroll, 0-i);
                    delay(5);
                }
                delay(1000);
                lcd.Vert_Scroll(0, max_scroll, 0);
                if(!(rotation&1)) {
                    lcd.Print("ONLY THE COLOR BAND",0,200,2,BLUE, BLACK,1); 
                    for(i=1;i<=64;i++) {
                        lcd.Vert_Scroll(48, 64, i);
                        delay(5);
                    }
                    delay(1000);
                    lcd.Vert_Scroll(0, max_scroll, 0);
                }
                lcd.Print("SOFTWARE SCROLL",0,216,2,YELLOW, BLACK,1); 
                for (int16_t i = lcd.Width, dx = 4, dy = 0; i > 0; i -= dx) {
                    windowScroll(0, 216, lcd.Width, 16, dx, dy, scrollbuf);
                    delay(5);
                }
                delay(2000);
            }
            
            lcd.Invert_Display(true);
            delay(2000);
            lcd.Invert_Display(false);
        }
    }
};
disp_s ds;