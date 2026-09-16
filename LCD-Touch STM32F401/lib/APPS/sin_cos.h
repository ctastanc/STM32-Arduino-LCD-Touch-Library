#include <LCD_KBV.h> 

void fast_sin_cos(void);

static uint16_t *sbuf = new uint16_t[318];
static uint16_t *cbuf = new uint16_t[318];
static uint16_t *sin_table = new uint16_t[360];
static uint16_t *cos_table = new uint16_t[360];

void init_tables() {
    for (int i = 0; i < 360; i++) {
        // Calculation of radians and placement at the center (119)
        float rad = (float)i * 0.01745329f; // (PI / 180)
        sin_table[i] = (int16_t)(119 + sinf(rad) * 90);
        cos_table[i] = (int16_t)(119 + cosf(rad) * 90);
    }
}

void fast_sin_cos(void) {
    static int delta = 0;
    static int frame_count = 0;
    lcd.Fill_Screen(BLACK);
    lcd.Set_Rotation(LANDSCAPE);
    lcd.Print("Sin", 110, 1, 2, CYAN);
    lcd.Print("Cos", 175, 1, 2, YELLOW);
    lcd.Fast_VLine(159, 15, 210, BLUE);
    lcd.Fast_HLine(1,  119, 318, BLUE);
    init_tables();
    while(1) {
        uint16_t start = micros();
        CS_L;
        for (int i = 0; i < 318; i++) {
            uint16_t ns = sin_table[(i + delta) % 360];
            uint16_t nc = cos_table[(i + delta) % 360];
            if(i != 159) SET_X(i,i); // col address set
            if (sbuf[i] != ns && ns!=119) {
                SET_Y(sbuf[i],sbuf[i]);// page address set
                CMD8(MW); DATA16(BLACK); // 0x2C  write memory
                SET_Y(sbuf[i]=ns,sbuf[i]); 
                CMD8(MW); DATA16(CYAN);
            }
            if (cbuf[i] != nc && nc!=119) {
                SET_Y(cbuf[i],cbuf[i]);
                CMD8(MW); DATA16(BLACK); 
                SET_Y(cbuf[i]=nc,cbuf[i]);
                CMD8(MW); DATA16(YELLOW); 
            }
        }
        delta = (delta +1) % 360;
        uint16_t elapsed_us = micros()-start ;
        if (++frame_count >=40) {
            frame_count = 0;
            lcd.Print(elapsed_us, 5, 1, 2, RED, BLACK, 1);
            lcd.Print("us", 55, 1, 2, RED);
            Serial.println(elapsed_us);
        }
        delay(5); //speed up/down
    }
}
