Arduino Mega
<img width="1262" height="1041" alt="Arduino Mega" src="https://github.com/user-attachments/assets/10e3bdf5-ef3a-4ff4-8751-64343c3bb448" />
STM32F401
<img width="1239" height="839" alt="STM32F401" src="https://github.com/user-attachments/assets/4d259987-92c5-48ad-bebd-11ccfa07f51d" />
This code was created using the LCDWIKI-Arduino library with some modifications. It is approximately 100 times faster than the Arduino Mega with the STM32F401.

The library is compatible with the ILI9341, ILI9325, ILI9328, HX8357D, HX8347G, HX8347I, ILI9486, ST7735S, ILI9488, and ILI9481 LCDs. However, it has not been tested with LCDs other than the ILI9341.

It has also been tested with the ILI9341 (2.4 inch MAR 2406) LCD on STM32F103 and WeAct BlackPill STM32F401 MCUs.

The LCD-Touch STM32F103 and LCD-Touch STM32F401 folders can be run directly in VsCode-Platformio. Alternatively, you can add the library files from the main directory to your project.

In the mcu_regs.h file, specify the frequency of the MCU you are working with using CPU_F. This is because the latencies set for MCUs faster than 84MHz depend on this frequency.
