## STM32 Arduino LCD & Touch Driver Library

An ultra-fast, high-performance, and bare-metal optimized parallel 8/16-bit TFT LCD and 4-wire resistive touch screen 
library for STM32 microcontrollers under the Arduino framework. 

This library is specifically designed to eliminate the bloated and sluggish nature of generic Arduino display libraries. 
By utilizing **STM32 Low-Layer (LL) drivers**, direct register manipulation (BSRR), loop unrolling, and compile-time 
optimizations, it pushes the hardware parallel bus to its physical transmission limits. 

---

## 📌 Key Features & Performance Architecture
* **Zero-Overhead Address Window:** The critical functions Set_Addr_Window and Draw_Pixel are decorated with 
	`__attribute__((always_inline))` inline, forcing the compiler to expand them in-place. This eliminates function 
	call overhead (PUSH/POP cycles) and accelerates frame/outline drawing by **~25%**.
* **STM32F103 Bug Protection:** Implements an advanced preprocessor architecture to completely avoid the notorious 
	STM32F1xx LL library bug where using output pull configurations could silently drop the pin speed to 10MHz. 
	Pins are locked at **50MHz** for F103 and maximum frequency for F401.
* **Loop Unrolling & Batching:** Uses `__attribute__((optimize("unroll-loops")))` combined with a Duff's Device style 
	8-pixel batching (BLOCK8) method, reducing loop branching overhead to absolute zero during screen/rectangle filling.
* **Smart Noise Filtering for Touch:** Features an oversampling engine with NUMSAMPLES == 2 tolerance matching 
	(±2 ADC counts verification) or median insertion sort filtering to eliminate analog signal noise without lagging the CPU.
* **Overclock Ready:** Safely handle aggressive MCU overclocking (e.g., STM32F401 running at 108MHz).

---

### ⚡ Single-Cycle Hardware Parallel Bus Write (`WRITE8`)

The absolute core of the library's speed relies on a tightly optimized hardware-software synergy inside the pixel-pumping macro:

```cpp
#define WRITE8(d) { DATA_PORT1->BSRR = DATA_MASK1 | (d); WR_H; }
```

#### Why is this so fast?
1. **Direct Register Manipulation (`BSRR`):** Instead of using slow functions, this macro 
	directly modifies the **Bit Set/Reset Register (BSRR)** of the STM32 GPIO port. This allows the MCU to clear old 
	data pins, reset WR pin and set the new 8-bit pixel data (`d`) **in a single CPU clock cycle**.
2. **Sequential Pin Alignment:** Because `D0-D7` are sequentially aligned on `PA0-PA7`, no expensive runtime bit-shifting
	or bitmasking calculations are needed. The raw data byte matches the lower port bits perfectly.
3. **Hardware-Paced Clock Toggling:** The display's Write Clock (`WR`) pin is placed right next to the data pins on `PA8`. 
	Toggling the clock (`WR_H`) is executed in the exact same port context.

---

## 🎨 Zero-Cost RGB565 Color Engine (Usage)

The library features an optimized, packed `RGB` structure that handles standard 24-bit (R,G,B) to 16-bit (RGB565) color 
conversion **at compile-time** using `constexpr`. Thanks to the `uint16_t` operator overloading, you can use raw hex 
values and the `RGB` structure interchangeably with absolute zero runtime CPU cost.
```h
struct RGB {
    uint16_t val;
    constexpr RGB(uint16_t c) : val(c) {}
    constexpr RGB(uint8_t r, uint8_t g, uint8_t b) : val(((uint16_t)(r&0xF8)<<8) | ((uint16_t)(g&0xFC)<<3) | (b>>3)) {} 
    constexpr operator uint16_t() const { return val; }
}__attribute__((packed));

template <typename T> 	
    void Print(T val, int16_t x, int16_t y, uint8_t size, const RGB& fc, const RGB& bc=0, bool mode=0, int16_t system = 10, uint8_t dec = 2)
```

### Dynamic & Static Usage Examples:

```cpp
// 1. Define pre-processor macros for legacy support
#define WHITE 0xFFFF
#define BLACK 0x0000

void loop() {
    // Both styles compile to the exact same high-speed assembly code!
    
    // Style A: Traditional raw HEX macros
    lcd.Print("xyz", 0, 0, 1, WHITE);
    
    // Style B: Clean, human-readable RGB constructor (Computed at compile-time!)
    lcd.Print("xyz", 0, 0, 1, RGB(255, 255, 255));
    
    // You can also use it inline inside any drawing functions:
    lcd.Fill_Rectangle_WH(10, 10, 50, 50, RGB(240, 10, 150));
}
```

## 🛠️ Supported Hardware
* **Microcontroller:** Fully compatible with STM32F1, STM32F4, and other popular STM32 boards.
* **Display Driver:**  ILI9341, ILI9325, ILI9328, ILI9481, ILI9486, ILI9488, ST7735S, HX8357D, HX8347G, HX8347I.
	The library includes a unified multi-driver template (lcd_regs.h) that automatically handles 
	different command architectures based on compile-time selections:
* **Touch Mechanism:** 4-Wire Resistive Touch (Connected directly to control and data pins).

---

## 📊 Benchmarks (STM32F401 Blackpill @ 108MHz Overclock)

The following metrics prove the massive performance jump compared to standard display engines. 
A full **240x320 screen fill takes only 2.8 milliseconds**, yielding a theoretical limit of **357 FPS** over the parallel bus. 

| Benchmark					| Time (Microseconds)	| Execution Style 				| 
| -------------------------	| --------------------:	| ----------------------------- | 
| Screen Fill(240x320)		|		 2,891 µs 		| Ultra-Fast Batching (357 FPS)	|
| Cross Line(400 pixel)		|		   310 µs 		| Direct Port Write				|
| Line(320 pixel)			| 			16 µs 		| Fully Inlined Address Window	|
| Fast Line(320 pixel)		| 			16 µs 		| Zero Function Call Overhead	|
| Rectangle(WxH) 			| 			54 µs 		| Unrolled Loops				|
| Fill Rectangle(WxH)		| 		 3,380 µs 		| Zero Branching				|
| Circle(R=240)				| 		   594 µs 		| Fast Box Drawing				|
| Fill Circle(R=240)		| 		 2,332 µs 		| High Bandwidth Pixel Pump		|
| Triangle(320x288x288)		| 		   469 µs 		| High Bandwidth Pixel Pump		|
| Fill Triangle(320x288x288)| 		 2,085 µs 		| High Bandwidth Pixel Pump		|
| Round Rectangle(WxH) 		| 		    77 µs 		| High Bandwidth Pixel Pump		|
| Fill Round Rectangle(WxH)	| 		 3,370 µs 		| High Bandwidth Pixel Pump		|

The "colligate_test()" function results:

| colligate_test();     | Standard Code(Arduino Mega)	| This Code(STM32F401)	|         Difference        |
| --------------------- | ---------------------------:	| --------------------:	| ------------------------: |
| show text             | 	163,224 µs					|  1,058 µs				|   163224/1058 = 154 times |
| show fill rectangle   | 	 95,412 µs         			|    724 µs				| 	  95412/724 = 131 times |
| show fill round rect	|   139,020 µs         			|  1,088 µs				|   139020/1088 = 127 times |
| show fill circle      |   161,296 µs         			|  1,303 µs				|   161296/1303 = 123 times |
| show fill triangle    |   141,516 µs         			|  1,220 µs				|   141516/1220 = 115 times |
| show grid lines       | 4,567,916 µs       			| 22,452 µs				| 4567916/22452 = 203 times |
| show random pixels    | 6,003,092 µs       			| 36,826 µs				| 6003092/36826 = 163 times |
---

## 🔌 Pin Connection (Wiring)

For maximum performance, **the 8-bit Data Bus (D0-D7) and the Write Pin (WR) must be kept sequentially on the same Port (e.g., PORTA)**. 
This allows the driver to dump 8 bits of data and toggle the clock pin in a single clock cycle using the BSRR register. 

### 8-Bit Extension Data Bus(PORTA)

| Display Pin | STM32 Pin | Description |
| ----------- | --------- | ----------- |
| D0          | PA0       | Touch XP	|
| D1          | PA1       | Touch YM	|
| D2          | PA2       | 		  	|
| D3          | PA3       |   			|
| D4          | PA4       |   			|
| D5          | PA5       |  			|
| D6          | PA6       |   			|
| D7          | PA7       | 			|
| WR          | PA8       | Write Clock (Critical Sequence)	|

### Control Bus(PORTB)

| Display Pin | STM32 Pin | Description / Touch Mapping |
| ----------- | --------- | --------------------------- |
| RS / CD     | PB0       | Register Select (Touch XM)  |
| CS          | PB1       | Chip Select (Touch YP)      |
| RD          | PB14      | Read Clock					|
| RST         | PB15      | Reset						|

### Optionel 16-Bit Extension Data Bus(PORTB)

| Display Pin | STM32 Pin |
| ----------- | --------- |
| D8          | PB3       |
| D9          | PB4       |
| D10         | PB5       |
| D11         | PB6       |
| D12         | PB7       |
| D13         | PB8       |
| D14         | PB9       |
| D15         | PB10      |

⚠️ **Note for STM32F103 (Bluepill):**
PB3 and PB4 are assigned to JTAG by default. The driver automatically disables JTAG via LL_GPIO_AF_Remap_SWJ_NOJTAG() inside the initialization sequence to release these pins for the 16-bit LCD bus, while safely keeping SWD active for debugging.

### Touch Pins

| Touch Pin| STM32 Pin |
| -------- | --------- |
| XM       | PB0       |
| YP       | PB1       |
| XP       | PA0       |
| YM       | PA1       |

⚠️ **Important for Touch(ILI9341):**
For the resistive touch to function correctly, XM (PB0) and YP (PB1) must be analog-capable pins. XP maps to PA0 and YM maps to PA1.

---

## 💻 VS Code + PlatformIO Setup

### Project Configuration
To use this library in your PlatformIO project, make sure your `platformio.ini` file is configured correctly. Here is a production-ready example for the WeAct BlackPill STM32F401 84Mhz. STLINK-V3 was used; 'COM7' should be replaced with the virtual COM port found in your device list. 

| STLINK V3 COM Port	| STM32 Pin |
| ---------------------	| --------- |
| RX       				| PA9(TX)	|
| TX       				| PA10(RX)	|

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

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com).

---

## 📜 License
This project is [MIT](LICENSE) licensed.

