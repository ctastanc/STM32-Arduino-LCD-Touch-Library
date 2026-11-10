#include <LCD_SRW.h>
#include <SYS_OC.h>
#include <sin_cos.h>
#include <phonecall.h>
#include <touchdemo.h>
#include <colligate_test.h>
#include <Meter_Linear.h>
#include <meters.h>
#include <cube_demo.h>
#include <clock_analog.h>
#include <test_lcd.h>
#include <pong.h>
#include <touch_calibrate.h>
#include <rotate_rects.h>
#include <test_read.h>
#include <text_up_left_scrool.h>

void setup(void) {
    SystemClock_OC(OC_108MHz); // read sys_oc.h for the warning.
    Serial.begin(115200);
    analogReadResolution(12);
    lcd.Init();
}

void loop(void) {
//***TOUCH*******************/ 
    //touch_calibration();
    //touch_demo();
    //phonecall();
//***READ********************/
    //test_read();
//***OTHERS******************/
    //Meter_Linear();
    //clock_analog();
    //meters();
    //fast_sin_cos();
    //cube_demo();
    //rotate_rect();
    //text_UpScrool();
    //text_LeftScrool();
    //pong();
//***TEST*******************/
    //colligate_test();
    test_lcd();

}
