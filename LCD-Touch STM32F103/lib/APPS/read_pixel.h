#include <LCD_KBV.h> 

class readp{
    public:
 
    void color_dump(uint16_t x,uint16_t y);
    void read_pixel();

};

extern readp rp;

void readp::color_dump(uint16_t x,uint16_t y)
{
    char buf[30] = {0},pbuf[10] = {0};
    uint8_t wd = (lcd.Width - 9 * 6)/ (5 * 6);
    uint8_t hi = (lcd.Height / 8) - 1;
    uint16_t pixel = 0;

    for(int j = 0;j< hi;j++)
    {
        sprintf(buf,"%3d,%3d:",x,y+j);
        lcd.Print(buf, 0, 8*(j+1)*lcd.Get_Text_Size(),1,WHITE);
        for(int i=0;i<wd;i++)
        {          
            //read pixel
            pixel = lcd.Read_Pixel(x+i,y+j);
            // if white set green
            if(WHITE == pixel) lcd.Set_Text_Color(GREEN); 
            sprintf(pbuf,"%04X ",pixel); 
            lcd.Print(pbuf,(strlen(buf)+strlen(pbuf)*i)*6*lcd.Get_Text_Size(),8*(j+1)*lcd.Get_Text_Size(),1,lcd.Get_Text_Color());
            //set white
            lcd.Set_Text_Color(WHITE);
        } 
    }
}   

//uint8_t aspect;
//char *aspectname[] = {"PORTRAIT", "LANDSCAPE", "PORTRAIT_REV", "LANDSCAPE_REV"};
uint16_t colors[] = {BLACK, BLUE};

void readp::read_pixel() 
{
    uint16_t iter, color;
    char buf[30];
    //aspect = (aspect + 1) & 3;
    //lcd.Set_Rotation((uint8_t)aspect);
    lcd.Set_Rotation(LANDSCAPE);
 
    for (iter = 0; iter < sizeof(colors) / sizeof(uint16_t); iter++) 
    {
        color = colors[iter];
        lcd.Fill_Screen(color);
        sprintf(buf, " ID=0x%04X Background=%04X %s", lcd.Read_ID(), color);//, aspectname[aspect]);
        lcd.Print(buf,0,0,1,WHITE,color);
        color_dump(36,0);
        delay(3000);

    } 

}