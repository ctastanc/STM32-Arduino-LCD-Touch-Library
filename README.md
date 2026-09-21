# STM32 Arduino LCD Touch Library

An ultra-fast, high-performance, and bare-metal optimized parallel 8/16-bit TFT LCD and 4-wire resistive touch screen library for STM32 microcontrollers under the Arduino framework.This library is specifically designed to eliminate the bloated and sluggish nature of generic Arduino display libraries. By utilizing STM32 Low-Layer (LL) drivers, direct register manipulation (BSRR), loop unrolling, and compile-time optimizations, it pushes the hardware parallel bus to its physical transmission limits.

---

# 🚀 Key Features & Performance Architecture

Zero-Overhead Address Window: The critical functions Set_Addr_Window and Draw_Pixel are decorated with __attribute__((always_inline)) inline, forcing the compiler to expand them in-place. This eliminates function call overhead (PUSH/POP cycles) and accelerates frame/outline drawing by ~25%.
STM32F103 Bug Protection: Implements an advanced preprocessor architecture to completely avoid the notorious STM32F1xx LL library bug where using output pull configurations could silently drop the pin speed to 10MHz. Pins are locked at 50MHz for F103 and maximum frequency for F401.
Loop Unrolling & Batching: Uses __attribute__((optimize("unroll-loops"))) combined with a Duff's Device style 8-pixel batching (BLOCK8) method, reducing loop branching overhead to absolute zero during screen/rectangle filling.
Smart Noise Filtering for Touch: Features an oversampling engine with NUMSAMPLES == 2 tolerance matching (±2 ADC counts verification) and median insertion sort filtering to eliminate analog signal noise without lagging the CPU.
Overclock Ready: Includes dynamically adjusted inline assembly NOP delays to safely handle aggressive MCU overclocking (e.g., STM32F401 running at 96MHz).
---

## 🛠️ Supported Hardware
* **Microcontroller:** Fully compatible with STM32F1, STM32F4, and other popular STM32 boards.
* **Display Driver:**  ILI9341, ILI9325, ILI9328, HX8357D, HX8347G, HX8347I, ILI9486, ST7735S, ILI9488, and ILI9481.
* **Touch Mechanism:** 4-Wire Resistive Touch (Connected directly to control and data pins).

---

## 🔌 Pin Connection (Wiring)

According to the library's default configuration, the pins must be connected as follows:

### Data Pins (8-Bit Parallel)

| Display Pin | STM32 Pin | Port Pin |
| ----------- | --------- | -------- |
| D0          | PA0       | Port A0  |
| D1          | PA1       | Port A1  |
| D2          | PA2       | Port A2  |
| D3          | PA3       | Port A3  |
| D4          | PA4       | Port A4  |
| D5          | PA5       | Port A5  |
| D6          | PA6       | Port A6  |
| D7          | PA7       | Port A7  |

For 16 Bit Parallel
| Display Pin | STM32 Pin | Port Pin |
| ----------- | --------- | -------- |
| D8          | PB3       | Port B3  |
| D9          | PB4       | Port B4  |
| D10         | PB5       | Port B5  |
| D11         | PB6       | Port B6  |
| D12         | PB7       | Port B7  |
| D13         | PB8       | Port B8  |
| D14         | PB9       | Port B9  |
| D15         | PB10      | Port B10 |

### Control & Touch Pins

| Display Pin | STM32 Pin | Description / Touch Mapping |
| ----------- | --------- | --------------------------- |
| WR          | PA8       | Write Clock Pin             |
| RS / CD     | PB0       | Register Select (Touch XM)  |
| CS          | PB1       | Chip Select (Touch YP)      |
| RD          | PB14      | Read Clock Pin              |
| RST         | PB15      | Reset Pin                   |

*⚠️ **Important for Touch(ILI9341):** For the resistive touch to function correctly, **XM (PB0)** and **YP (PB1)** must be analog-capable pins. **XP** maps to **PA0** and **YM** maps to **PA1**.*

| Touch Pin| STM32 Pin |
| -------- | --------- |
| XM       | PB0       |        
| YP       | PB1       |
| XP       | PA0       |
| YM       | PA1       |
---

## 💻 VS Code + PlatformIO Setup

### 1. Project Configuration
To use this library in your PlatformIO project, make sure your `platformio.ini` file is configured correctly. Here is a production-ready example for the WeAct BlackPill STM32F401 84Mhz.:

```ini
[env:blackpill_f401ce]
platform = ststm32
board = blackpill_f401ce
framework = arduino
board_build.mcu = stm32f401ceu6
board_build.f_cpu = 84000000L
upload_protocol = stlink
debug_tool = stlink
monitor_port = COM7
monitor_speed = 115200
build_flags =
  	-ffast-math
    -flto
```

### 2. Quick Start Example
Create or update your `src/main.cpp` file with the following setup to test the display and analog touch:

```cpp
#include <LCD_KBV.h> 
#include <sys_oc.h>

const uint16_t cols[16] ={ BLUE, RED, GREEN, CYAN, MAGENTA,  YELLOW, WHITE, ORANGE, DARKGREEN, DARKCYAN,
                            MAROON,PURPLE,OLIVE,LIGHTGREY, GREENYELLOW,PINK};

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

void read_test() {
    for(int i= 0; i<16;i++){
        lcd.Pixel(50, i*15+10 ,cols[i]);
        int p=lcd.Read_Pixel(50,i*15+10);
        lcd.Print(p,70,i*15+5,1,p);
        lcd.Print((String)("ILI"+String(lcd.Read_ID(),HEX)),160,110,2,GREEN);
    }
}

void setup(void) {
     /**********************************************************************
       ATTENTION:
       This line applies to STM32F401 devices equipped with a 25MHz crystal and 
       STM32F103 devices equipped with 8MHz crystal. 
       Do not use it if you lack sufficient knowledge about your hardware.
       Adverse results may occur. You bear full responsibility. */
    //SystemClock_OC(OC_96MHz); // Overclock to 96MHz
    /**********************************************************************/
    analogReadResolution(12);
    Serial.begin(115200);
    delay(150);
    Serial.println("system running...");
    set_pin_output(GPIOC, LL_GPIO_PIN_13);
    lcd.Init_LCD();
    lcd.Fill_Screen(BLACK);
    lcd.Set_Rotation(LANDSCAPE);
}

void loop(void) {
    //rotate_rect();
    //lcd.Print_HScroll((uint8_t*)("What will happen in the future is also one of the mysteries of the universe."),1,1,lcd.Height-125,2,RED,BLACK,10);
    //lcd.Print(String(12345,BIN),CENTER,100,2,RED);
    //full_screen_test();
    read_test();
}
```
The library is very fast. The full-screen test takes 3.2ms.

| full_screen_test(); |  time  |
| ------------------- | ------ |
| Full Screen         | 3.2 ms |

The colligate_test(); results:

| colligate_test();       | Arduino Mega       | STM32F401  | Difference                |
| ----------------------- | ------------------ | ---------- | ------------------------- |
| show text               | 163224 us          | 1194 us    | 163224/1194   = 136 times |
| show fill rectangle     | 95412 us           | 827 us     | 95412/827     = 115 times |
| show fill round rect    | 139020 us          | 1552 us    | 139020/1552   = 89  times |
| show fill circle        | 161296 us          | 2013 us    | 161296/2013   = 80 times  |
| show fill triangle      | 141516 us          | 1499 us    | 141516/1499   = 94 times  |
| show grid lines         | 4567916 us         | 53603 us   | 4567916/53603 = 85 times  |
| show random pixels      | 6003092 us         | 60081 us   | 6003092/60081 = 99 times  |
---

Mega <img width="640" height="435" alt="mega-640" src="https://github.com/user-attachments/assets/e2a9142d-84ca-463a-ab4c-ebc2c97e61dd" />

STM <img width="640" height="472" alt="stm-640" src="https://github.com/user-attachments/assets/ca761a8e-09ce-470b-85e1-a07d9c2e4cdf" />


## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com).

---

## 📜 License
This project is [MIT](LICENSE) licensed.

