## STM32 Arduino LCD & Touch Driver Library

An ultra-fast, high-performance, and bare-metal optimized parallel 8/16-bit TFT LCD and 4-wire resistive touch screen library for STM32 microcontrollers under the Arduino framework. 

This library is specifically designed to eliminate the bloated and sluggish nature of generic Arduino display libraries. By utilizing **STM32 Low-Layer (LL) drivers**, direct register manipulation (BSRR), loop unrolling, and compile-time optimizations, it pushes the hardware parallel bus to its physical transmission limits. 

---

## 📌 Key Features & Performance Architecture
* **Zero-Overhead Address Window:** The critical functions Set_Addr_Window and Draw_Pixel are decorated with __attribute__((always_inline)) inline, forcing the compiler to expand them in-place. This eliminates function call overhead (PUSH/POP cycles) and accelerates frame/outline drawing by **~25%**.
* **STM32F103 Bug Protection:** Implements an advanced preprocessor architecture to completely avoid the notorious STM32F1xx LL library bug where using output pull configurations could silently drop the pin speed to 10MHz. Pins are locked at **50MHz** for F103 and maximum frequency for F401.
* **Loop Unrolling & Batching:** Uses __attribute__((optimize("unroll-loops"))) combined with a Duff's Device style 8-pixel batching (BLOCK8) method, reducing loop branching overhead to absolute zero during screen/rectangle filling.
* **Smart Noise Filtering for Touch:** Features an oversampling engine with NUMSAMPLES == 2 tolerance matching (±2 ADC counts verification) and median insertion sort filtering to eliminate analog signal noise without lagging the CPU.
* **Overclock Ready:** Includes dynamically adjusted inline assembly NOP delays to safely handle aggressive MCU overclocking (e.g., STM32F401 running at 96MHz).

---

## 🛠️ Supported Hardware
* **Microcontroller:** Fully compatible with STM32F1, STM32F4, and other popular STM32 boards.
* **Display Driver:**  ILI9341, ILI9325, ILI9328, ILI9481, ILI9486, ILI9488, ST7735S, HX8357D, HX8347G, HX8347I.
	The library includes a unified multi-driver template (lcd_regs.h) that automatically handles different command architectures based on compile-time selections:
* **Touch Mechanism:** 4-Wire Resistive Touch (Connected directly to control and data pins).

---

## 📊 Benchmarks (STM32F401 Blackpill @ 96MHz Overclock)

The following metrics prove the massive performance jump compared to standard display engines. A full **240x320 screen fill takes only 3.2 milliseconds**, yielding a theoretical limit of **312 FPS** over the parallel bus. 

| Benchmark					| TestTime (Microseconds)	| Execution Style 				| 
| -------------------------	| ------------------------- | ----------------------------- | 
| Screen Fill (240x320)		| 		 3,253 µs 			| Ultra-Fast Batching (312 FPS)	|
| Horiz/Vert Lines			| 		 1,705 µs 			| Direct Port Write				|
| Lines (Outline)			| 		32,076 µs 			| Fully Inlined Address Window	|
| Circles (Outline)			| 		12,876 µs 			| Zero Function Call Overhead	|
| Circles (Filled)			| 		15,462 µs 			| Unrolled Loops				|
| Triangles (Outline)		| 		 7,014 µs 			| Zero Branching				|
| Rectangles (Outline)		| 		 1,244 µs 			| Fast Box Drawing				|
| Rectangles (Filled)		| 		38,502 µs 			| High Bandwidth Pixel Pump		|
| Rounded Rects (Outline)	| 		 4,815 µs 			| Inlined Math					|

The "colligate_test()" function results:

| colligate_test();       | Standard Code (Arduino Mega)| This Code (STM32F401)	| Difference                |
| ----------------------- | --------------------------- | ---------------------	| ------------------------- |
| show text               | 163,224 µs					| 1,194 µs				| 163224/1194   = 136 times |
| show fill rectangle     | 95,412 µs          			| 827 µs				| 95412/827     = 115 times |
| show fill round rect    | 139,020 µs         			| 1,552 µs				| 139020/1552   = 89  times |
| show fill circle        | 161,296 µs         			| 2,013 µs				| 161296/2013   = 80 times  |
| show fill triangle      | 141,516 µs         			| 1,499 µs				| 141516/1499   = 94 times  |
| show grid lines         | 4,567,916 µs       			| 53,603 µs				| 4567916/53603 = 85 times  |
| show random pixels      | 6,003,092 µs       			| 60,081 µs				| 6003092/60081 = 99 times  |
---

## 🔌 Pin Connection (Wiring)

For maximum performance, **the 8-bit Data Bus (D0-D7) and the Write Pin (WR) must be kept sequentially on the same Port (e.g., PORTA)**. This allows the driver to dump 8 bits of data and toggle the clock pin in a single clock cycle using the BSRR register. 

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

⚠️ **Note for STM32F103 (Bluepill):** PB3 and PB4 are assigned to JTAG by default. The driver automatically disables JTAG via LL_GPIO_AF_Remap_SWJ_NOJTAG() inside the initialization sequence to release these pins for the 16-bit LCD bus, while safely keeping SWD active for debugging.

### Touch Pins

| Touch Pin| STM32 Pin |
| -------- | --------- |
| XM       | PB0       |        
| YP       | PB1       |
| XP       | PA0       |
| YM       | PA1       |

⚠️ **Important for Touch(ILI9341):** For the resistive touch to function correctly, XM (PB0) and YP (PB1) must be analog-capable pins. XP maps to PA0 and YM maps to PA1.
---

## 💻 VS Code + PlatformIO Setup

### Project Configuration
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

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com).

---

## 📜 License
This project is [MIT](LICENSE) licensed.

