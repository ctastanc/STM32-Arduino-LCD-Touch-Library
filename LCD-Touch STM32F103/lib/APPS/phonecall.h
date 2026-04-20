#include <TouchScreen.h> 
#include <LCD_KBV.h> 

void phonecall();

#define BTN_R 25 //the radius of button 
#define BTN_SPC_X 25 //the horizontal distance between button
#define BTN_SPC_Y 5  //the vertical distance between button
#define EDG_Y 5 //lower edge distance 
#define EDG_X 20 //left and right distance

typedef struct _button_info {
    uint8_t name[10];
    uint8_t name_size;
    uint16_t name_color;
    uint16_t color;
    int16_t x;
    int y;     
}button_info;

button_info button[15]; 

void Initialize_Buttons() {
    uint16_t h = lcd.Height;
    button[0] = {"1",3,BLACK,CYAN,EDG_X+BTN_R-1,h-EDG_Y-4*BTN_SPC_Y-9*BTN_R-1};
    button[1] = {"2",3,BLACK,CYAN,EDG_X+3*BTN_R+BTN_SPC_X-1,h-EDG_Y-4*BTN_SPC_Y-9*BTN_R-1};
    button[2] = {"3",3,BLACK,CYAN,EDG_X+5*BTN_R+2*BTN_SPC_X-1,h-EDG_Y-4*BTN_SPC_Y-9*BTN_R-1};
    button[3] = {"4",3,BLACK,CYAN,EDG_X+BTN_R-1,h-EDG_Y-3*BTN_SPC_Y-7*BTN_R-1};
    button[4] = {"5",3,BLACK,CYAN,EDG_X+3*BTN_R+BTN_SPC_X-1,h-EDG_Y-3*BTN_SPC_Y-7*BTN_R-1};
    button[5] = {"6",3,BLACK,CYAN,EDG_X+5*BTN_R+2*BTN_SPC_X-1,h-EDG_Y-3*BTN_SPC_Y-7*BTN_R-1};
    button[6] = {"7",3,BLACK,CYAN,EDG_X+BTN_R-1,h-EDG_Y-2*BTN_SPC_Y-5*BTN_R-1};
    button[7] = {"8",3,BLACK,CYAN,EDG_X+3*BTN_R+BTN_SPC_X-1,h-EDG_Y-2*BTN_SPC_Y-5*BTN_R-1};
    button[8] = {"9",3,BLACK,CYAN,EDG_X+5*BTN_R+2*BTN_SPC_X-1,h-EDG_Y-2*BTN_SPC_Y-5*BTN_R-1};
    button[9] = {"*",3,BLACK,PINK,EDG_X+BTN_R-1,h-EDG_Y-BTN_SPC_Y-3*BTN_R-1};
    button[10] = {"0",3,BLACK,CYAN,EDG_X+3*BTN_R+BTN_SPC_X-1,h-EDG_Y-BTN_SPC_Y-3*BTN_R-1};
    button[11] = {"#",3,BLACK,PINK,EDG_X+5*BTN_R+2*BTN_SPC_X-1,h-EDG_Y-BTN_SPC_Y-3*BTN_R-1};
    button[12] = {"end",2,BLACK,RED,EDG_X+BTN_R-1,h-EDG_Y-BTN_R-1};
    button[13] = {"call",2,BLACK,GREEN,EDG_X+3*BTN_R+BTN_SPC_X-1,h-EDG_Y-BTN_R-1};
    button[14] = {"del",2,BLACK,LIGHTGREY,EDG_X+5*BTN_R+2*BTN_SPC_X-1,h-EDG_Y-BTN_R-1};
};

bool is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py) {
    if((px > x1 && px < x2) && (py > y1 && py < y2)) return true;  
    else return false;  
}

void show_menu(void) {
   for(uint16_t i = 0;i < sizeof(button)/sizeof(button_info);i++) {
        lcd.Fill_Circle(button[i].x, button[i].y, BTN_R,button[i].color);
        uint8_t str_len = strlen((const char *)(button[i].name));
        lcd.Print(button[i].name, button[i].x-str_len *button[i].name_size*6/2+1,
        button[i].y-button[i].name_size*8/2+1, button[i].name_size, 
        button[i].name_color);
   }
}

void draw_pressed(uint16_t i, uint16_t bc, uint16_t tc) {
    uint8_t str_len = strlen((const char *)button[i].name);
    lcd.Fill_Circle(button[i].x, button[i].y, BTN_R, bc);
    lcd.Print(button[i].name, button[i].x- str_len*button[i].name_size*6/2+1,
    button[i].y-button[i].name_size*8/2+1, button[i].name_size,tc);
}

void phonecall() {
    lcd.Fill_Screen(BLACK);
    lcd.Set_Rotation(PORTRAIT);
    Initialize_Buttons();
    uint16_t text_x=10,text_y=6, n=0;
    uint16_t text_x_add = 6*button[0].name_size, text_y_add = 8*button[0].name_size;
    show_menu();
    while(true){
        digitalWrite(PC13, HIGH);
        TSPoint p = ts.getPoint();
        digitalWrite(PC13, LOW);
        if (p.z)
        {
            /*Serial.print("\tPressure = "); Serial.println(p.z);
            Serial.print("   X = "); Serial.print(p.x); Serial.print("\tY = "); Serial.println(p.y);*/
            for(uint16_t i=0;i<sizeof(button)/sizeof(button_info);i++) {
                if(is_pressed(button[i].x-BTN_R, button[i].y-BTN_R, button[i].x+BTN_R, 
                              button[i].y+BTN_R, p.x, p.y)) {
                    draw_pressed(i,DARKGREY, WHITE);   
                    delay(100);
                    draw_pressed(i,button[i].color, button[i].name_color); 
                    if(i < 12) {
                        if(n < 13) {
                        lcd.Print(button[i].name, text_x, text_y, button[i].name_size, GREENYELLOW);
                        text_x += text_x_add-1;
                        n++;}
                    }
                    else if(12 == i) {
                        lcd.Fill_Rectangle(0, 33, lcd.Width-1, 42,BLACK);
                        lcd.Print("Calling ended",CENTER,33,1,RED);} 
                    else if(13 == i) {
                        lcd.Fill_Rectangle(0, 33, lcd.Width-1, 42,BLACK);
                        lcd.Print("Calling...",CENTER,33,1,GREEN);}
                    else if(14 == i) {
                        lcd.Fill_Rectangle(0, 33, lcd.Width-1, 42,BLACK);
                        if(n > 0) {
                            text_x -= (text_x_add-1);  
                            lcd.Fill_Rectangle(text_x, text_y, text_x+text_x_add-1, text_y+text_y_add-2,BLACK);
                            n--;}
    }   }   }   }   }   
}