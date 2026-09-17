/***********************************************************************************
*
* This demo was made for LCD modules with 8bit data port.
* STM32 - LCD 8-bit parallel LCD + TouchScreen
* Hardware Environment: STM32 Arduino STM32F401
* Build Environment   : STM32Arduino
*
*=========================== pin usage as follow: ================================
*                  LCD_CS  LCD_RS  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK
*     STM32          B1      B0      PA8    PB14     PB15
*     Touch          YP      XM
*
*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7
*     STM32          PA0     PA1     PA2     PA3     PA4     PA5     PA6     PA7
*     Touch           XP     YM
*=================================================================================
*
* LCD_SYS_INTERFACE, MCU_SPEED, LCD_MODEL and PIN DEFNITIONS are defined in setting.h
*
**********************************************************************************/

#include <LCD_KBV.h> 
#include <sin_cos.h>
#include <phonecall.h>
#include <touchdemo.h>
#include <bench2.h>
#include <read_pixel.h>
#include <switch_test.h>
#include <colligate_test.h>
#include <Meter_Linear.h>
#include <meters.h>
#include <cube_demo.h>
#include <clock_analog.h>
#include <disp_scroll.h>

#include <pong.h>
#include <Scroll_Test.h>
#include <bench1.h>
#include <sys_conf.h>

void full_screen_test() {
    uint32_t start = micros(); 
    lcd.Fill_Screen(BLUE); 
    uint32_t elapsed_us = micros() - start;
    Serial.println(String(elapsed_us)+ " us"); 
    digitalWrite(PC13, HIGH);
    lcd.Print((String)("MCU "+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,YELLOW,BLUE,0);
    lcd.Print((String)(String(elapsed_us)+" us"),CENTER,130,4,YELLOW,BLUE,0);
    lcd.Print((String)("APB1 "+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,205,3,YELLOW,BLACK,0);
    delay(1000);
    lcd.Fill_Screen(RED); 
    digitalWrite(PC13, LOW);
    lcd.Print((String)("MCU "+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,YELLOW,BLUE,0);
    lcd.Print((String)(String(elapsed_us)+" us"),CENTER,130,4,YELLOW,BLUE,0);
    lcd.Print((String)("APB1 "+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,205,3,YELLOW,BLACK,0);
    delay(1000);
}

void rotate_rect(void) { 
    const uint16_t cols[16] ={ BLUE, RED, GREEN, CYAN, MAGENTA,  YELLOW, WHITE, ORANGE,
    DARKGREEN, DARKCYAN,MAROON,PURPLE,OLIVE,LIGHTGREY, GREENYELLOW,PINK};
    lcd.Set_Rotation(LANDSCAPE);
    while(1) {
        srand(time(0));
        int rn = cols[(rand() % 16) ];
        lcd.Rectangle_WH(0,0,lcd.Width,lcd.Height,YELLOW);
        lcd.Fast_HLine(35,120,245,BLUE);
        lcd.Fast_VLine(160,35,175,BLUE);
        lcd.Fill_Circle(160,120,40,BLACK);
        lcd.Circle(160,120,40,YELLOW);
        for(int x=1;x<3;x++){ 
            for(int x=1;x<91;x++){ 
                lcd.Fill_Rotated_Rectangle(160,120,50,50,x,rn);
                lcd.Fill_Rotated_Rectangle(80,60,50,50,90-x,rn);
                lcd.Fill_Rotated_Rectangle(240,180,50,50,90-x,rn);
                for(int i=2;i<5;i=i+2) {
                    lcd.Rotated_Rectangle(160,120,50+i,50+i,x,BLACK);
                    lcd.Rotated_Rectangle(80,60,50+i,50+i,90-x,BLACK);
                    lcd.Rotated_Rectangle(240,180,50+i,50+i,90-x,BLACK);
                }
            }
        }
        int rn1 = cols[(rand() % 16) ];
        for(int x=90;x>=0;x--){
            lcd.Fill_Rotated_Rectangle(160,120,50,50,x,rn1);
            lcd.Fill_Rotated_Rectangle(240,60,50,50,90-x,rn1);
            lcd.Fill_Rotated_Rectangle(80,180,50,50,90-x,rn1);
            for(int i=2;i<5;i=i+2) {
                lcd.Rotated_Rectangle(160,120,50+i,50+i,x,BLACK);
                lcd.Rotated_Rectangle(240,60,50+i,50+i,90-x,BLACK);
                lcd.Rotated_Rectangle(80,180,50+i,50+i,90-x,BLACK);
            }
        }
    }
}

void test_read() {
    const uint16_t cols[16] ={ BLUE, RED, GREEN, CYAN, MAGENTA,  YELLOW, WHITE, ORANGE,
    DARKGREEN, DARKCYAN,MAROON,PURPLE,OLIVE,LIGHTGREY, GREENYELLOW,PINK};
    for(int i= 0; i<16;i++){
        lcd.Pixel(50, i*15+10 ,cols[i]);
        int p=lcd.Read_Pixel(50,i*15+10);
        lcd.Print(p,70,i*15+5,1,p);
        lcd.Print((String)("ILI"+String(lcd.Read_ID(),HEX)),160,110,2,GREEN);
    }
}

void setup(void) {
    SystemClock_OC(OC_96MHz); // Overclock to 96MHz
    Serial.begin(115200);
    delay(150);
    Serial.println("system running...");
    analogReadResolution(10); // STM32 ADC default 12-bit; touch 10-bit
    pinMode(PC13,OUTPUT);
    lcd.Init_LCD();
    lcd.Fill_Screen(BLACK);
    lcd.Set_Rotation(LANDSCAPE);
}

void loop(void) {
    //rotate_rect();
    //lcd.Print_HScroll((uint8_t*)("What will happen in the future is also one of the mysteries of the universe."),1,1,lcd.Height-125,2,RED,BLACK,10);
    //lcd.Print(String(12345,BIN),CENTER,100,2,RED);
    //full_screen_test();
    //fast_sin_cos();
    //touch_demo();
    //phonecall();
    b2.bench2();
    //b1.bench1();
    //ml.Meter_Linear();
    //cl.clock_analog();
    //mt.meters();
    //ds.disp_scroll();
    //cb.cube_demo();
    //rp.read_pixel();
    //sw.switch_test();
    //colligate_test(); 

    //Scroll_Test();
    //pn.pong();
    //test_read();
}