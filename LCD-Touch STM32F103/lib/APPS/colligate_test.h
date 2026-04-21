#include <LCD_KBV.h> 

void colligate() ;
unsigned long show_triangle_function(void);
//display main surface
unsigned long show_text(void)
{
    unsigned long time_start = micros();
    lcd.Fill_Rectangle(0, 0, lcd.Width-1, 14,RGB(32, 0,255));
    lcd.Print("* Universal Color TFT Display Library *",CENTER,3,1,0x07E0);

    lcd.Fill_Rectangle(0, lcd.Height-15, lcd.Width-1, lcd.Height-1,RGB(128, 128, 128));
    lcd.Print("---> http://www.lcdwiki.com <---",CENTER,lcd.Height-11,1,0xFFFF);

    lcd.Rectangle(0, 15, lcd.Width-1, lcd.Height-16,RGB(32, 0, 255));   
    return micros() - time_start;
}

//display triangle functions
unsigned long show_triangle_function(void)
{
     uint16_t i;
     unsigned long time_start = micros();
     // Draw crosshairs
     lcd.Fast_VLine(lcd.Width/2-1, 16, lcd.Height- 32,RGB(0, 0, 255));
     lcd.Fast_HLine(1, lcd.Height/2-1, lcd.Width-2,RGB(0, 0, 255));
     for(i = 1;i <= (lcd.Height- 32)/2/10;i++)
     {
         lcd.Fast_HLine(lcd.Width/2-1-2, lcd.Height/2-1-i*10, 5,RGB(0, 0, 255));
         lcd.Fast_HLine(lcd.Width/2-1-2, lcd.Height/2-1+i*10, 5,RGB(0, 0, 255));
     }
     for(i = 1;i <= (lcd.Width-2)/2/10;i++)
     {
         lcd.Fast_VLine(lcd.Width/2-1-i*10, lcd.Height/2-1-2, 5,RGB(0, 0, 255));
         lcd.Fast_VLine(lcd.Width/2-1+i*10, lcd.Height/2-1-2, 5,RGB(0, 0, 255));
     }
     
     // Draw sin lines
     lcd.Print("sin",5,17,1,0x07FF, 0,0);
     for (i=1; i<lcd.Width-2; i++)
     {
        lcd.Pixel(i,lcd.Height/2-1+(sin(((i*1.13)*3.14)/180)*95),RGB(0, 255, 255));
     }

     // Draw cos lines
     lcd.Print("cos",5,25,1,0x07E0, 0,0);
     for (i=1; i<lcd.Width-2; i++)
     {
        lcd.Pixel(i,lcd.Height/2-1+(cos(((i*1.13)*3.14)/180)*95),RGB(0, 255, 0));
     }

     // Draw tan lines
     lcd.Print("tan",5,33,1,0xFFE0, 0,0);
     for (i=1; i<lcd.Width-2; i++)
     {
        lcd.Pixel(i,lcd.Height/2-1+(tan(((i*1.13)*3.14)/180)*10),RGB(255, 255, 0));
     }

     // Draw cot lines
     lcd.Print("cot",5,41,1,0xF800, 0,0);
     for (i=1; i<lcd.Width-2; i++)
     {
        lcd.Pixel(i,lcd.Height/2-1+1/(tan(((i*1.13)*3.14)/180)*0.1),RGB(255, 0, 0));
     }
     return micros()-time_start;
}

// Draw a moving sinewave
unsigned long show_sinewave(void)
{
   uint16_t buf[lcd.Width-2],x = 1,i,y;
   unsigned long time_start = micros();
   int16_t wid = lcd.Width;
   int16_t t;
   float k;
   if(wid == 320)
   {
      t = 20;
      k = 1.1; 
   }
   else
   {
     t = 15;
     k = 0.7;  
   }
   lcd.Fast_VLine(lcd.Width/2-1, 16, lcd.Height- 32,RGB(0, 0, 255));
   lcd.Fast_HLine(1, lcd.Height/2-1, lcd.Width-2,RGB(0, 0, 255));
   for (i=1; i<((lcd.Width-2)*t); i++) 
   {
      x++;
      if (x==lcd.Width-1)
      {
          x=1;
      }
      if (i>lcd.Width-1)
      {
          if ((x==lcd.Width/2-1)||(buf[x-1]==lcd.Height/2-1))
          {
              lcd.Pixel(x,buf[x-1],RGB(0, 0, 255));
          }
          else
          {
             lcd.Pixel(x,buf[x-1],RGB(0, 0, 0));
          }
          
    }
    y=lcd.Height/2-1+(sin(((i*k)*3.14)/180)*(90-(i/100)));
    lcd.Pixel(x,y,RGB(255, 64, 255));
    buf[x-1]=y;
  }
  return micros()- time_start;   
}

// Draw some filled rectangles
unsigned long show_fill_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (lcd.Height-40)/5;
    uint16_t x_spec = (lcd.Width-5*side_len)/2;
    uint16_t y_spec = (lcd.Height-5*side_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              lcd.Set_Draw_Color(RGB(255,0,255));
              break;
            case 1:
              lcd.Set_Draw_Color(RGB(255,0,0));
              break;
            case 2:
              lcd.Set_Draw_Color(RGB(0,255,0));
              break;
            case 3:
              lcd.Set_Draw_Color(RGB(0,0,255));
              break;
            case 4:
              lcd.Set_Draw_Color(RGB(255,255,0));
              break;
            default:
              break;
          }   
          lcd.Fill_Rectangle(x_spec+i*side_len-1, y_spec+i*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(i+1)*side_len-1,lcd.Get_Draw_color());
          lcd.Fill_Rectangle(x_spec+i*side_len-1, y_spec+(5-i)*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(4-i)*side_len-1,lcd.Get_Draw_color()); 
     }
     return micros()- time_start;   
}

// Draw some filled round rectangles
unsigned long show_fill_round_rectangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t side_len = (lcd.Height-40)/5;
    uint16_t x_spec = (lcd.Width-5*side_len)/2;
    uint16_t y_spec = (lcd.Height-5*side_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              lcd.Set_Draw_Color(RGB(255,0,255));
              break;
            case 1:
              lcd.Set_Draw_Color(RGB(255,0,0));
              break;
            case 2:
              lcd.Set_Draw_Color(RGB(0,255,0));
              break;
            case 3:
              lcd.Set_Draw_Color(RGB(0,0,255));
              break;
            case 4:
              lcd.Set_Draw_Color(RGB(255,255,0));
              break;
            default:
              break;
          }   
          lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+i*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(i+1)*side_len-1,10,lcd.Get_Draw_color());
          lcd.Fill_Round_Rectangle(x_spec+i*side_len-1, y_spec+(5-i)*side_len-1, x_spec+(i+1)*side_len-1, y_spec+(4-i)*side_len-1,10,lcd.Get_Draw_color()); 
     }
     return micros()- time_start;   
}

// Draw some filled circles
unsigned long show_fill_circle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t r_len = (lcd.Height-40)/5/2;
    uint16_t x_spec = (lcd.Width-5*r_len*2)/2;
    uint16_t y_spec = (lcd.Height-5*r_len*2)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              lcd.Set_Draw_Color(RGB(255,0,255));
              break;
            case 1:
              lcd.Set_Draw_Color(RGB(255,0,0));
              break;
            case 2:
              lcd.Set_Draw_Color(RGB(0,255,0));
              break;
            case 3:
              lcd.Set_Draw_Color(RGB(0,0,255));
              break;
            case 4:
              lcd.Set_Draw_Color(RGB(255,255,0));
              break;
            default:
              break;
          }   
          lcd.Fill_Circle(x_spec+r_len+i*r_len*2-1, y_spec+r_len+i*r_len*2-1,r_len,lcd.Get_Draw_color());
          lcd.Fill_Circle(x_spec+r_len+i*r_len*2-1, y_spec+(5-i)*r_len*2-r_len-1,r_len,lcd.Get_Draw_color()); 
     }
     return micros()- time_start;   
 }

// Draw some filled triangles
unsigned long show_fill_triangle(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    uint16_t h_len = (lcd.Height-40)/5;
    uint16_t side_len = (h_len*115)/100;
    uint16_t x_spec = (lcd.Width-5*side_len)/2;
    uint16_t y_spec = (lcd.Height-5*h_len)/2;
    for(i = 0;i<5;i++)
    {
          switch (i)
          {
            case 0:
              lcd.Set_Draw_Color(RGB(255,0,255));
              break;
            case 1:
              lcd.Set_Draw_Color(RGB(255,0,0));
              break;
            case 2:
              lcd.Set_Draw_Color(RGB(0,255,0));
              break;
            case 3:
              lcd.Set_Draw_Color(RGB(0,0,255));
              break;
            case 4:
              lcd.Set_Draw_Color(RGB(255,255,0));
              break;
            default:
              break;
          } 
          lcd.Fill_Triangle(x_spec+i*side_len-1,y_spec+(i+1)*h_len-1,x_spec+side_len/2+i*side_len-1,y_spec+i*h_len-1,x_spec+(i+1)*side_len-1,y_spec+(i+1)*h_len-1,lcd.Get_Draw_color()); 
          lcd.Fill_Triangle(x_spec+i*side_len-1,y_spec+(5-i)*h_len-1,x_spec+side_len/2+i*side_len-1,y_spec+(4-i)*h_len-1,x_spec+(i+1)*side_len-1,y_spec+(5-i)*h_len-1,lcd.Get_Draw_color());  
     }
     return micros()- time_start;   
}

// Draw some lines in a pattern
unsigned long show_grid_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    int16_t wid = lcd.Width;
   float k;
   if(wid == 320)
   {
      k = 1.44; 
    }
    else
    {
     k = 1.6;  
    }
    for (i=16; i<lcd.Height-17; i+=5)
    {
       lcd.Line(1, i, (i*k)-10, lcd.Height-17,RGB(255,0,0));
     }
     for (i=lcd.Height-17; i>16; i-=5)
    {
      lcd.Line(lcd.Width-2, i, (i*k)-11, 16,RGB(255,0,0));
    }
    for (i=lcd.Height-16; i>16; i-=5)
    {
        lcd.Line(1, i, (lcd.Height-17)*k+10-(i*k), 16,RGB(0,255,255));
    }
    for (int i=15; i<lcd.Height-17; i+=5)
    {
        lcd.Line(lcd.Width-2, i, (lcd.Height-17)*k+10-(i*k), lcd.Height-17,RGB(0,255,255));
    }
    return micros()- time_start;   
}

// Draw some random pixels
unsigned long show_random_pixels(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 10000;i++)
    {
       lcd.Pixel(2+random(lcd.Width-4),17+random(lcd.Height-34),RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random lines
unsigned long show_random_lines(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 300;i++)
    {
       lcd.Line(2+random(lcd.Width-4),17+random(lcd.Height-34),2+random(lcd.Width-4),17+random(lcd.Height-34),RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random rectangles
unsigned long show_random_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       lcd.Rectangle(2+random(lcd.Width-4),17+random(lcd.Height-34),2+random(lcd.Width-4),17+random(lcd.Height-34),RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random round rectangles
unsigned long show_random_round_rectangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       lcd.Round_Rectangle(2+random(lcd.Width-4),17+random(lcd.Height-34),2+random(lcd.Width-4),17+random(lcd.Height-34),5,RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random circles
unsigned long show_random_circles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       lcd.Circle(41+random(lcd.Width-82), 56+random(lcd.Height-112), random(40),RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random triangles
unsigned long show_random_triangles(void)
{
    uint16_t i;
    unsigned long time_start = micros();
    for(i = 0;i< 150;i++)
    {
       lcd.Triangle(2+random(lcd.Width-4),17+random(lcd.Height-34),2+random(lcd.Width-4),17+random(lcd.Height-34),2+random(lcd.Width-4),17+random(lcd.Height-34),RGB(random(255),random(255),random(255)));
    }
    return micros()- time_start; 
}

// Draw some random bit maps
unsigned long show_random_bit_map(void)
{
    uint16_t buf[48],i;
    unsigned long time_start = micros();
    int16_t len = (lcd.Height*3/4)/6;
    for(i = 0;i< 48; i++)
    {
       lcd.Set_Draw_Color(RGB(random(255), random(255), random(255)));
       buf[i] = lcd.Get_Draw_color();
    }
    for(i = 1;i<=6;i++)
    {
        lcd.Draw_Bit_Map(lcd.Width/2-1-((len/2)*4/3)*i, lcd.Height/2-1-(len/2)*i, 8, 6, buf, i*(len/6));
        delay(100);
    }
    return micros()- time_start; 
}

//Clear the screen
void clear_screen(void)
{
   delay(2000);  
   lcd.Fill_Rectangle(1, 16, lcd.Width-2, lcd.Height-17,RGB(0, 0, 0));
}

unsigned long (*show_function[])(void) = 
 {
      show_text,
      show_triangle_function,
      show_sinewave,
      show_fill_rectangle,
      show_fill_round_rectangle,
      show_fill_circle,
      show_fill_triangle,
      show_grid_lines,
      show_random_pixels,
      show_random_lines,
      show_random_rectangles,
      show_random_round_rectangles,
      show_random_circles,
      show_random_triangles,
      show_random_bit_map,
};
uint8_t *show_str[]= {
    (uint8_t *)"show text :", 
    (uint8_t *)"show triangle function :",
    (uint8_t *)"show sinewave :",
    (uint8_t *)"show fill rectangle :",
    (uint8_t *)"show fill round rectangle :",
    (uint8_t *)"show fill circle :",
    (uint8_t *)"show fill triangle :",
    (uint8_t *)"show grid lines :",
    (uint8_t *)"show random pixels :",
    (uint8_t *)"show random lines :",
    (uint8_t *)"show random rectangles :",
    (uint8_t *)"show random round rectangles :",
    (uint8_t *)"show random circles :",
    (uint8_t *)"show random triangles :",
    (uint8_t *)"show random bit_map :"
};

//display the running time of programs 
unsigned long show_total_time(void)
{
     uint16_t i;
     unsigned long buf[15];
     unsigned long time_start = micros();
     for(i = 0;i< 15;i++)
     {
        buf[i] = show_function[i](); 
        clear_screen();  
     }
     for(i = 0;i<15; i++)
     {
         lcd.Print(show_str[i],(lcd.Width-260)/2-1,(lcd.Height-150)/2+i*10-1,1,0xFD20, 0,0);
         lcd.Set_Text_Color(RGB(0, 255, 0)); 
         lcd.Print_Number_Int(buf[i], (lcd.Width-260)/2-1+200, (lcd.Height-150)/2+i*10-1, 0, ' ', 10);
     }
     delay(25000);
     return micros()- time_start; 
}

//display ending and total running time
void show_end(unsigned long run_time)
{
    lcd.Fill_Screen(RGB(0, 255, 255));
    lcd.Fill_Round_Rectangle(lcd.Width/2-1-120+1, lcd.Height/2-1-60+1, lcd.Width/2-1+120-1, lcd.Height/2-1+60-1,5,RGB(255, 0, 0));
    lcd.Print("Running over!", CENTER, lcd.Height/2-1-40,1,RGB(0,255,255));
    lcd.Print("That's ok!", CENTER, lcd.Height/2-1-30,1,RGB(0,255,255));
    lcd.Print("After a few seconds,", CENTER, lcd.Height/2-1-20,1,RGB(0,255,255));
    lcd.Print("it will restart.", CENTER, lcd.Height/2-1-10,1,RGB(0,255,255));
    lcd.Print("Please wait ...", CENTER, lcd.Height/2-1,1,RGB(0,255,255));
    lcd.Print("Total runtime(us):  ", lcd.Width/2-1-90, lcd.Height/2-1+40,1,RGB(255,255,0));
    lcd.Set_Text_Color(RGB(0, 255, 0));
    lcd.Print_Number_Int(run_time, lcd.Width/2-1+30, lcd.Height/2-1+40, 0, ' ', 10);  
    delay(10000);   
}

void colligate() 
{
    unsigned long total_time;
    lcd.Set_Rotation(1); 
    lcd.Fill_Screen(0x0); 
    total_time = show_total_time();
    show_end(total_time);    
}
