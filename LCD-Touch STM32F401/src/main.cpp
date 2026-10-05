/****************************** pin usage as follow ************************************
*     ILI9341      LCD_CS  LCD_RS  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK
*     STM32          B1      B0      PA8    PB14     PB15
*     Touch          YP      XM
*
*     ILI9341      LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7
*     STM32          PA0     PA1     PA2     PA3     PA4     PA5     PA6     PA7
*     Touch           XP     YM
****************************************************************************************
* LCD_MODEL, LCD_SYS_INTERFACE and PIN DEFNITIONS are defined in setting.h
****************************************************************************************/

#include <LCD_SRW.h> 
#include <sys_oc.h>
#include <sin_cos.h>
#include <phonecall.h>
#include <touchdemo.h>
#include <test1.h>
#include <read_pixel.h>
#include <switch_test.h>
#include <colligate_test.h>
#include <Meter_Linear.h>
#include <meters.h>
#include <cube_demo.h>
#include <clock_analog.h>
#include <disp_scroll.h>
#include <Scroll_Test.h>
#include <test_lcd.h>
#include <pong.h>
#include <touch_calibrate.h>
#include <rotate_rects.h>
#include <test_read.h>

void full_screen_test() {
    uint32_t start = micros(); 
    lcd.Fill_Screen(BLUE); 
    uint32_t elapsed_us = micros() - start;
    Serial.print("Full Screen   : "); Serial.println(String(elapsed_us)+ " us"); 
    lcd.Print((String)("Frequency:"+String(SystemCoreClock/1000000) + " MHz"),CENTER,55,3,WHITE,BLUE,0);
    lcd.Print((String)("Full Screen:"+String(elapsed_us)+" us"),CENTER,100,3,WHITE,BLUE,0);
    lcd.Print((String)("PCLK1:"+String(HAL_RCC_GetPCLK1Freq()/1000000) +" Mhz"),CENTER,145,3,WHITE,BLACK,0);
    lcd.Print((String)("PCLK2:"+String(HAL_RCC_GetPCLK2Freq()/1000000) +" Mhz"),CENTER,180,3,WHITE,BLACK,0);
    while(true);
}

void text_scrool() { 
    const char* text = "What will happen in the future is also one of the mysteries of the universe.";
    lcd.Print_LeftScroll(text, lcd.Width, lcd.Height-125, 2, RED, BLACK, 10);
}

void setup(void) {
/*****************************************************************************
    ATTENTION:
    This line applies to STM32F401 devices equipped with a 25MHz crystal and 
    STM32F103 devices equipped with 8MHz crystal. 
    Do not use it if you lack sufficient knowledge about your hardware.
    Adverse results may occur. You bear full responsibility. */
    SystemClock_OC(OC_108MHz); 
/*****************************************************************************/
    analogReadResolution(12);
    Serial.begin(115200);
    delay(150);
    Serial.println("system running...");
    set_pin_output(GPIOC, LL_GPIO_PIN_13);
    digital_write(GPIOC,LL_GPIO_PIN_13,1);
    lcd.Init();
    lcd.Cls(BLACK);
    lcd.Set_Rotation(LANDSCAPE);
}

void loop(void) {
/***TOUCH********************/ 
    //touch_calibration();
    //touch_demo();
    //phonecall();
    //switch_test();
//***READ********************/
    //read_pixel();
    //test_read();
//***OTHERS*****************/
    //Meter_Linear();
    //clock_analog();
    //meters();
    //display_scroll();
    //fast_sin_cos();
    //Scroll_Test();
    //cube_demo();
    //rotate_rect();
    //text_scrool();
    //pong();
//***TEST*******************/
    //colligate_test();
    test_lcd();
    //test1();
    //full_screen_test();
    //lcd.Print(String(12345,BIN),CENTER,100,2,RED);
}
