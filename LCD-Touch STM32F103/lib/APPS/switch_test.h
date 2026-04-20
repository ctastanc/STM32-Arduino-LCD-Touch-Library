#include <TouchScreen.h>
#include <LCD_KBV.h>
#include <switch_font.h>

class swi{
    public:
    void switch_test();
    void main_menu(void);
    void show_system_menu(void);
    bool is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py);
    void show_picture( uint8_t *color_buf,int16_t buf_size,int16_t x1,int16_t y1,int16_t x2,int16_t y2);
};

extern swi sw;

bool switch_flag_1 = true,switch_flag_2 = true,switch_flag_3 = true,switch_flag_4 = true,switch_flag_5 = true,switch_flag_6 = true;  
int16_t menu_flag = 1,old_menu_flag;     

//TouchScreen ts = TouchScreen(295);
//ts(295);
//display string

//diaplay a picture
void swi::show_picture( uint8_t *color_buf,int16_t buf_size,int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
    SET_ADDR_WINDOW(x1, y1, x2, y2); 
    lcd.Push_Any_Color(color_buf, buf_size, 1, 1);
}

//Check whether to press or not
bool swi::is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))  return true;  
     else return false;
}

//show the system menu
void swi::show_system_menu(void)
{    
    lcd.Print("time setting",10,150,2,BLACK, BLACK,1);
    lcd.Print("date setting",10,170,2,BLACK, BLACK,1);
    lcd.Print("alarm setting",10,190,2,BLACK, BLACK,1);
    lcd.Print("talk setting",10,210,2,BLACK, BLACK,1);
    lcd.Print("sound setting",10,230,2,BLACK, BLACK,1);
    switch(menu_flag)
    {
        case 1:
        {
            lcd.Fill_Rectangle(0, 147, lcd.Width-1, 166,BLUE);
            lcd.Print("time setting",10,150,2,WHITE, BLACK,1);
            break;
        }
        case 2:
        {
            lcd.Fill_Rectangle(0, 167, lcd.Width-1, 186,BLUE);
            lcd.Print("date setting",10,170,2,WHITE, BLACK,1);
            break;
        }
        case 3:
        {
            lcd.Fill_Rectangle(0, 187, lcd.Width-1, 206,BLUE);
            lcd.Print("alarm setting",10,190,2,WHITE, BLACK,1);
            break;
        }
        case 4:
        {
            lcd.Fill_Rectangle(0, 207, lcd.Width-1, 226,BLUE);
            lcd.Print("talk setting",10,210,2,WHITE, BLACK,1);
            break;
        }
        case 5:
        {
            lcd.Fill_Rectangle(0, 227, lcd.Width-1, 246,BLUE);
            lcd.Print("sound setting",10,230,2,WHITE, BLACK,1);
            break;
        }
        default:
        break;
    }
}
                    
void swi::main_menu(void) 
{    
    Serial.println(lcd.Read_ID(), HEX);
    lcd.Fill_Screen(WHITE); 

    //show the switch picture
    lcd.Set_Draw_Color(LIGHTGREY);//(192, 192, 192);
    lcd.Fast_HLine(0, 3, lcd.Width,LIGHTGREY);
    sw.show_picture((uint8_t*)switch_on_2,sizeof(switch_on_2)/2,5,5,34,34);
    lcd.Print("switch is on ",60,11,2,GREEN, BLACK,1);

    lcd.Fast_HLine(0, 37, lcd.Width,LIGHTGREY);
    
    lcd.Print("wifi setting",5,40,2,BLACK, BLACK,1);
    show_picture((uint8_t*)switch_on_3,sizeof(switch_on_3)/2,195,40,234,54);

    lcd.Fast_HLine(0, 57, lcd.Width,LIGHTGREY);
    
    lcd.Print("bt setting",5,60,2,BLACK, BLACK,1);
    show_picture((uint8_t*)switch_on_3,sizeof(switch_on_3)/2,195,60,234,74);

    lcd.Fast_HLine(0, 77, lcd.Width,LIGHTGREY);
    
    lcd.Print("auto time",5,80,2,BLACK, BLACK,1);
    show_picture((uint8_t*)switch_on_1,sizeof(switch_on_1)/2,204,80,218,94);

    lcd.Fast_HLine(0, 97, lcd.Width,LIGHTGREY);
    
    lcd.Print("enable lock",5,100,2,BLACK, BLACK,1);
    show_picture((uint8_t*)switch_on_1,sizeof(switch_on_1)/2,204,100,218,114);

    lcd.Fast_HLine(0, 116, lcd.Width,LIGHTGREY);
    
    lcd.Print("system setting   >",5,119,2,BLUE, BLACK,1);
    lcd.Fast_HLine(0, 138, lcd.Width,LIGHTGREY);
}

void swi::switch_test(void)
{
    sw.main_menu();
    while(1){
        digitalWrite(13, HIGH);
        TSPoint p = ts.getPoint();
        digitalWrite(13, LOW);

        if (p.z )
        {
            if(sw.is_pressed(5,5,34,34,p.x,p.y))
            {
                if(switch_flag_1)
                {   //display off 
                    sw.show_picture((uint8_t*)switch_off_2,sizeof(switch_off_2)/2,5,5,34,34);
                    lcd.Fill_Rectangle(60, 11,  216, 27,WHITE);
                    lcd.Print("switch is off ",60,11,2,RED, BLACK,1); 
                    switch_flag_1 = false;
                }
                else
                {  //display on
                sw.show_picture((uint8_t*)switch_on_2,sizeof(switch_on_2)/2,5,5,34,34);
                lcd.Fill_Rectangle(60, 11,  216, 27,WHITE);
                lcd.Print("switch is on ",60,11,2,GREEN, BLACK,1);
                switch_flag_1 = true;
                }
                delay(100);
            }
            if(sw.is_pressed(195,40,234,54,p.x,p.y))
            {
                if(switch_flag_2)
                {
                    sw.show_picture((uint8_t*)switch_off_3,sizeof(switch_off_3)/2,195,40,234,54);
                    switch_flag_2 = false;
                }
                else
                {
                sw.show_picture((uint8_t*)switch_on_3,sizeof(switch_on_3)/2,195,40,234,54);
                switch_flag_2 = true;
                }
                delay(100);
            }
            if(sw.is_pressed(195,60,234,74,p.x,p.y))
            {
                if(switch_flag_3)
                {
                    sw.show_picture((uint8_t*)switch_off_3,sizeof(switch_off_3)/2,195,60,234,74);
                    switch_flag_3 = false;
                }
                else
                {
                sw.show_picture((uint8_t*)switch_on_3,sizeof(switch_on_3)/2,195,60,234,74);
                switch_flag_3 = true;
                }
                delay(100);
            }
            if(sw.is_pressed(205,81,217,93,p.x,p.y))
            {
                if(switch_flag_4)
                {
                sw.show_picture((uint8_t*)switch_off_1,sizeof(switch_off_1)/2,204,80,218,94);
                    switch_flag_4 = false;
                }
                else
                {
                sw.show_picture((uint8_t*)switch_on_1,sizeof(switch_on_1)/2,204,80,218,94);
                    switch_flag_4 = true;
                }
                delay(100);
            }
            if(sw.is_pressed(205,101,217,113,p.x,p.y))
            {
                if(switch_flag_5)
                {
                sw.show_picture((uint8_t*)switch_off_1,sizeof(switch_off_1)/2,204,100,218,114);
                switch_flag_5 = false;
                }
                else
                {
                sw.show_picture((uint8_t*)switch_on_1,sizeof(switch_on_1)/2,204,100,218,114);
                switch_flag_5 = true;
                }
                delay(100);
            }
            if(sw.is_pressed(5,119,lcd.Width-1,137,p.x,p.y))
            {     
                lcd.Fill_Rectangle(0, 117, lcd.Width-1, 137,MAGENTA);
                delay(100);
                lcd.Fill_Rectangle(0, 117, lcd.Width-1, 137,WHITE);
                if(switch_flag_6)
                {
                    lcd.Print("system setting   <",5,119,2,BLUE, BLACK,1);
                    sw.show_system_menu();
                    switch_flag_6 = false;
                }
                else
                {
                    lcd.Print("system setting   >",5,119,2,BLUE, BLACK,1);
                    lcd.Fill_Rectangle(0, 147, lcd.Width-1, 250,WHITE);
                    switch_flag_6 = true;
                }
            }
            if(sw.is_pressed(0,147,lcd.Width-1,247,p.x,p.y)&&(!switch_flag_6))
            {
                old_menu_flag = menu_flag;     
                if(sw.is_pressed(0,147,lcd.Width-1,166,p.x,p.y))
                {
                    lcd.Fill_Rectangle(0, 147, lcd.Width-1, 166,BLUE);
                    lcd.Print("time setting",10,150,2,WHITE, BLACK,1);
                    menu_flag = 1;
                }
                if(sw.is_pressed(0,167,lcd.Width-1,186,p.x,p.y))
                {
                    lcd.Fill_Rectangle(0, 167, lcd.Width-1, 186,BLUE);
                    lcd.Print("date setting",10,170,2,WHITE, BLACK,1);
                    menu_flag = 2;
                }
                if(sw.is_pressed(0,187,lcd.Width-1,206,p.x,p.y))
                {
                    lcd.Fill_Rectangle(0, 187, lcd.Width-1, 206,BLUE);
                    lcd.Print("alarm setting",10,190,2,WHITE, BLACK,1);
                    menu_flag = 3;
                }
                if(sw.is_pressed(0,207,lcd.Width-1,226,p.x,p.y))
                {
                    lcd.Fill_Rectangle(0, 207, lcd.Width-1, 226,BLUE);
                    lcd.Print("talk setting",10,210,2,WHITE, BLACK,1);
                    menu_flag = 4;
                }
                if(sw.is_pressed(0,227,lcd.Width-1,246,p.x,p.y))
                {
                    lcd.Fill_Rectangle(0, 227, lcd.Width-1, 246,BLUE);
                    lcd.Print("sound setting",10,230,2,WHITE, BLACK,1);
                    menu_flag = 5;
                }  
                if(old_menu_flag != menu_flag)
                {
                    switch(old_menu_flag)
                    {
                        case 1:
                        {
                            lcd.Fill_Rectangle(0, 147, lcd.Width-1, 166,WHITE);
                            lcd.Print("time setting",10,150,2,BLACK, BLACK,1);
                            break;
                        }
                        case 2:
                        {
                            lcd.Fill_Rectangle(0, 167, lcd.Width-1, 186,WHITE);
                            lcd.Print("date setting",10,170,2,BLACK, BLACK,1);
                            break;
                        }
                        case 3:
                        {
                            lcd.Fill_Rectangle(0, 187, lcd.Width-1, 206,WHITE);
                            lcd.Print("alarm setting",10,190,2,BLACK, BLACK,1);
                            break;
                        }
                        case 4:
                        {
                            lcd.Fill_Rectangle(0, 207, lcd.Width-1, 226,WHITE);
                            lcd.Print("talk setting",10,210,2,BLACK, BLACK,1);
                            break;
                        }
                        case 5:
                        {
                            lcd.Fill_Rectangle(0, 227, lcd.Width-1, 246,WHITE);
                            lcd.Print("sound setting",10,230,2,BLACK, BLACK,1);
                            break;
                        }
                        default:
                        break;                  
                    }       
                }
                delay(100);
            }
        }
    }
}
