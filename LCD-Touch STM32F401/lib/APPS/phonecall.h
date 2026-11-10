#include <LCD_SRW.h>
#include <touch_screen.h>
#include <BUTTONS.h>

Buttons button[15];
Dual_Delay dd;
void buttonReleased();

void Initialize_Buttons() {
    button[0] =  Buttons(ROUND,20,45,50,50,CYAN,font30,"1",BLACK,3,buttonReleased);
    button[1] =  Buttons(ROUND,95,45,50,50,CYAN,font30,"2",BLACK,3,buttonReleased);
    button[2] =  Buttons(ROUND,170,45,50,50,CYAN,font30,"3",BLACK,3,buttonReleased);
    button[3] =  Buttons(ROUND,20,100,50,50,CYAN,font30,"4",BLACK,3,buttonReleased);
    button[4] =  Buttons(ROUND,95,100,50,50,CYAN,font30,"5",BLACK,3,buttonReleased);
    button[5] =  Buttons(ROUND,170,100,50,50,CYAN,font30,"6",BLACK,3,buttonReleased);
    button[6] =  Buttons(ROUND,20,155,50,50,CYAN,font30,"7",BLACK,3,buttonReleased);
    button[7] =  Buttons(ROUND,95,155,50,50,CYAN,font30,"8",BLACK,3,buttonReleased);
    button[8] =  Buttons(ROUND,170,155,50,50,CYAN,font30,"9",BLACK,3,buttonReleased);
    button[9] =  Buttons(ROUND,20,210,50,50,CYAN,font30,"*",BLACK,3,buttonReleased);
    button[10] = Buttons(ROUND,95,210,50,50,CYAN,font30,"0",BLACK,3,buttonReleased);
    button[11] = Buttons(ROUND,170,210,50,50,CYAN,font30,"#",BLACK,3,buttonReleased);
    button[12] = Buttons(ROUND,20,265,50,50,RED,font19,"End",BLACK,2,buttonReleased);
    button[13] = Buttons(ROUND,95,265,50,50,GREEN,font19,"Call",BLACK,2,buttonReleased);
    button[14] = Buttons(ROUND,170,265,50,50,DARKGREY,font19,"Del",BLACK,2,buttonReleased);
};

struct btn_disp_cont{ int tx, ty, tx_add, ty_add, i; } bd;

void show_main(void) {
    for(uint16_t i = 0;i < 15;i++) button[i].Draw();    
    lcd.Rectangle(1, 1, lcd.Width, lcd.Height,WHITE); lcd.Fast_HLine(1, 29, lcd.Width-2);
}

void buttonReleased() { 
    if(dd.first) button[bd.i].Draw();
    if(bd.i < 12) { if(bd.tx < 230) {
        lcd.PrintFont(font19,button[bd.i].name, bd.tx, bd.ty, button[bd.i].name_size,GREENYELLOW); bd.tx += bd.tx_add-1; }
    }
    else if(12 == bd.i) lcd.PrintFont(font9,"Calling ended",CENTER,33,1,RED,BLACK,1);
    else if(13 == bd.i) { lcd.Fill_Rectangle(2, 33, lcd.Width-4, 10,BLACK);
                          lcd.PrintFont(font9,"Calling...",CENTER,33,1,GREEN,BLACK,1);}
    else if(14 == bd.i) { lcd.Fill_Rectangle(2, 33, lcd.Width-4, 10,BLACK);
        if(bd.tx > 10) { bd.tx -= bd.tx_add-1; lcd.Fill_Rectangle(bd.tx, bd.ty, bd.tx_add, bd.ty_add-1,BLACK); }
    }   
 }

void phonecall() {
    lcd.Cls(BLACK); lcd.Set_Rotation(PORTRAIT);
    Initialize_Buttons(); show_main();
    TSPoint p;
    bd = {10, 6, 6*button[0].name_size-6, 8*button[0].name_size};
    dd = {600,100,true};
    while(true){
        p = ts.getPoint();
        if (p.v) {
            for(bd.i=0; bd.i<15; bd.i++) {
                if(button[bd.i].isPress(p.x, p.y)) {
                    button[bd.i].Pressed();
                    dd.set_delay();
                    while(p.z) {
                        p = ts.getPoint();
                        if(dd.get_delay()) if(button[bd.i].isPress(p.x, p.y)) button[bd.i].Released();
                    }
                    if(button[bd.i].isPress(p.x, p.y) && dd.first) button[bd.i].Released(); else button[bd.i].Cancel();
                    break;
                }   
            }  
        }  
    }   
}