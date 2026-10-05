#include <LCD_SRW.h> 

void fast_sin_cos(void);

static uint16_t *sbuf = new uint16_t[320];
static uint16_t *cbuf = new uint16_t[320];
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
        for (int i = 0; i < 320; i++) {
            uint16_t ns = sin_table[(i + delta) % 360];
            uint16_t nc = cos_table[(i + delta) % 360];
            if (sbuf[i] != ns && ns!=119 && i != 159) {
                lcd.Pixel(i,sbuf[i],BLACK); lcd.Pixel(i,sbuf[i]=ns,CYAN);
            }
            if (cbuf[i] != nc && nc!=119 && i != 159) {
                lcd.Pixel(i,cbuf[i],BLACK); lcd.Pixel(i,cbuf[i]=nc,YELLOW);
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
