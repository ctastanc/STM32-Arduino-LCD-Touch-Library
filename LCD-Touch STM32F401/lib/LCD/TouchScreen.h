// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'

#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#include <LCD_KBV.h>
#include <settings.h>

#if defined(STM32F0xx)
  #include "stm32f0xx_ll_adc.h"
#elif defined(STM32F1xx)
  #include "stm32f1xx_ll_adc.h"
#elif defined(STM32F2xx)
  #include "stm32f2xx_ll_adc.h"
#elif defined(STM32F3xx)
  #include "stm32f3xx_ll_adc.h"
#elif defined(STM32F4xx)
  #include "stm32f4xx_ll_adc.h"
#elif defined(STM32F7xx)
  #include "stm32f7xx_ll_adc.h"
#elif defined(STM32H7xx)
  #include "stm32h7xx_ll_adc.h"
#elif defined(STM32G0xx)
  #include "stm32g0xx_ll_adc.h"
#elif defined(STM32G4xx)
  #include "stm32g4xx_ll_adc.h"
#elif defined(STM32L0xx)
  #include "stm32l0xx_ll_adc.h"
#elif defined(STM32L1xx)
  #include "stm32l1xx_ll_adc.h"
#elif defined(STM32L4xx)
  #include "stm32l4xx_ll_adc.h"
#elif defined(STM32L5xx)
  #include "stm32l5xx_ll_adc.h"
#elif defined(STM32WBxx)
  #include "stm32wbxx_ll_adc.h"
#elif defined(STM32WLxx)
  #include "stm32wlxx_ll_adc.h"
#elif defined(STM32C0xx)
  #include "stm32c0xx_ll_adc.h"
#elif defined(STM32U0xx)
  #include "stm32u0xx_ll_adc.h"
#elif defined(STM32U5xx)
  #include "stm32u5xx_ll_adc.h"
#elif defined(STM32MP1xx)
  #include "stm32mp1xx_ll_adc.h"
#endif

struct TS_Pin {
    GPIO_TypeDef* port;
    uint32_t      pin;
    uint32_t      channel;
};

static inline void enable_adc_clock(ADC_TypeDef* ADCx) {
    (void)ADCx;
    #if defined(STM32F1xx) || defined(STM32F3xx)
        #if defined(LL_APB2_GRP1_PERIPH_ADC1)
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
        #endif
    #elif defined(STM32F2xx) || defined(STM32F4xx) || defined(STM32F7xx)
        #if defined(LL_APB2_GRP1_PERIPH_ADC1)
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
        #endif
    #elif defined(STM32L4xx) || defined(STM32L5xx) || defined(STM32G4xx) || defined(STM32WBxx)
        #if defined(LL_AHB2_GRP1_PERIPH_ADC12)
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
        #elif defined(LL_AHB2_GRP1_PERIPH_ADC)
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
        #endif
    #elif defined(STM32G0xx) || defined(STM32C0xx)
        #if defined(LL_APB2_GRP1_PERIPH_ADC)
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
        #elif defined(LL_APBENR2_ADCEN)
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
        #endif
    #elif defined(STM32F0xx)
        #if defined(LL_APB1_GRP2_PERIPH_ADC1)
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
        #endif
    #elif defined(STM32H7xx)
        #if defined(LL_AHB1_GRP1_PERIPH_ADC12)
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);
        #endif
    #endif

    #if defined(RCC_APB2ENR_ADC1EN)
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    #endif
}

static inline void ts_ll_adc_init(ADC_TypeDef* adc) {
    if (!adc) adc = ADC1;
    enable_adc_clock(adc);

#if defined(LL_ADC_SetResolution) && defined(LL_ADC_RESOLUTION_12B)
    LL_ADC_SetResolution(adc, LL_ADC_RESOLUTION_12B);
#endif

#if defined(LL_ADC_IsEnabled)
    if (!LL_ADC_IsEnabled(adc)) {
        LL_ADC_Enable(adc);
        for (volatile int i = 0; i < 2000; i++) { __NOP(); }
    }
#else
    if (!(adc->CR2 & ADC_CR2_ADON)) {
        adc->CR2 |= ADC_CR2_ADON;
        for (volatile int i = 0; i < 2000; i++) { __NOP(); }
    }
#endif

#if defined(LL_ADC_StartCalibration)
    if (LL_ADC_IsEnabled(adc)) {
        LL_ADC_StartCalibration(adc);
        while (LL_ADC_IsCalibrationOnGoing(adc)) {}
    }
#endif
}

// LL ADC single conversion
static inline uint16_t ts_ll_analog_read(ADC_TypeDef* adc, uint32_t channel) {
    if (!adc) adc = ADC1;
    ts_ll_adc_init(adc);

    // Assign the channel to Rank 1.
#if defined(LL_ADC_REG_SetSequencerRanks)
    LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_1, channel);
#elif defined(ADC_SQR3_SQ1)
    adc->SQR3 = (channel & 0x1FU);
#endif

    // Set Sampling Time
#if defined(LL_ADC_SetChannelSamplingTime)
  #if defined(LL_ADC_SAMPLINGTIME_56CYCLES)
    LL_ADC_SetChannelSamplingTime(adc, channel, LL_ADC_SAMPLINGTIME_56CYCLES);
  #elif defined(LL_ADC_SAMPLINGTIME_55CYCLES_5)
    LL_ADC_SetChannelSamplingTime(adc, channel, LL_ADC_SAMPLINGTIME_55CYCLES_5);
  #elif defined(LL_ADC_SAMPLINGTIME_47CYCLES_5)
    LL_ADC_SetChannelSamplingTime(adc, channel, LL_ADC_SAMPLINGTIME_47CYCLES_5);
  #elif defined(LL_ADC_SAMPLINGTIME_48CYCLES)
    LL_ADC_SetChannelSamplingTime(adc, channel, LL_ADC_SAMPLINGTIME_48CYCLES);
  #endif
#endif

    // Single Cycle (Sequencer Length = 1)
#if defined(LL_ADC_REG_SetSequencerLength)
    LL_ADC_REG_SetSequencerLength(adc, LL_ADC_REG_SEQ_SCAN_DISABLE);
#elif defined(ADC_SQR1_L)
    adc->SQR1 &= ~ADC_SQR1_L;
#endif

#if defined(LL_ADC_REG_StartConversionSWStart)
    LL_ADC_REG_StartConversionSWStart(adc);
#elif defined(LL_ADC_REG_StartConversion)
    LL_ADC_REG_StartConversion(adc);
#elif defined(ADC_CR2_SWSTART)
    adc->CR2 |= ADC_CR2_SWSTART;
#else
    adc->CR2 |= ADC_CR2_ADON;
#endif
    // Wait for the cycle to finish
    uint32_t timeout = 50000;
#if defined(LL_ADC_IsActiveFlag_EOC)
    while (!LL_ADC_IsActiveFlag_EOC(adc) && --timeout) {}
    LL_ADC_ClearFlag_EOC(adc);
#elif defined(LL_ADC_IsActiveFlag_EOCS)
    while (!LL_ADC_IsActiveFlag_EOCS(adc) && --timeout) {}
    LL_ADC_ClearFlag_EOCS(adc);
#elif defined(ADC_SR_EOC)
    while (!(adc->SR & ADC_SR_EOC) && --timeout) {}
    adc->SR &= ~ADC_SR_EOC;
#endif

    if (timeout == 0) return 0;
    // Read 12-bit Data
#if defined(LL_ADC_REG_ReadConversionData12)
    uint32_t val = LL_ADC_REG_ReadConversionData12(adc);
#else
    uint32_t val = adc->DR & 0x0FFFU;
#endif

    return (uint16_t)(val & 0x0FFFU);
}

// e-adc / external 12-bit ADC reading helper functions.
static inline uint16_t e_adc(uint32_t channel, ADC_TypeDef* adc = ADC1) {
    return ts_ll_analog_read(adc, channel);
}

static inline uint16_t ts_read_adc(uint32_t channel, ADC_TypeDef* adc = ADC1) {
    return ts_ll_analog_read(adc, channel);
}

static inline void ts_ll_pin_output(GPIO_TypeDef* port, uint32_t pin) {
    if (!port) return;
    enable_gpio_clock(port);
#if defined(LL_GPIO_MODE_OUTPUT_50MHz) // STM32F1
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT_50MHz);
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
#else // STM32F4, F7, G0, G4, H7, L4 vb.
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(port, pin, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO);
#endif
}

static inline void ts_ll_pin_analog(GPIO_TypeDef* port, uint32_t pin) {
    if (!port) return;
    enable_gpio_clock(port);
    LL_GPIO_SetPinMode(port, pin, LL_GPIO_MODE_ANALOG);
#if !defined(LL_GPIO_MODE_OUTPUT_50MHz)
    LL_GPIO_SetPinPull(port, pin, LL_GPIO_PULL_NO);
#endif
}

static inline void ts_ll_digital_write(GPIO_TypeDef* port, uint32_t pin, uint8_t val) {
    if (!port) return;
    if (val) {
        LL_GPIO_SetOutputPin(port, pin);
    } else {
        LL_GPIO_ResetOutputPin(port, pin);
    }
}

// Dokunmatik panel empedansının yerleşmesi için mikro saniye beklemesi
static inline void ts_settle_delay(void) {
    for (volatile int d = 0; d < 150; d++) {
        __NOP();
    }
}

//touch sensitivity for x
#define TS_MINX 500//500 (10-bit 125)
#define TS_MAXX 3500//3500 (10-bit 875)
//touch sensitivity for Y
#define TS_MINY 340//340 (10-bit 85)
#define TS_MAXY 3580//3580 (10-bit 895)
//touch sensitivity for press
#define MINPRESSURE 1000//200 (10-bit 50)
#define MAXPRESSURE 4000//4000 (10-bit 1000)
#define MAP(x,in_min,in_max,out_min,out_max) (((int32_t)(x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))

// Oversampling :
//   1 = no oversample 
//   2 = Double reading, if the two readings are not equal, consider it invalid (recommendation)
//   3+ = median value (insert sort)
#define NUMSAMPLES 20

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
    uint8_t j;
    int save;
    for (int i = 1; i < size; i++) {
        save = array[i];
        for (j = i; j >= 1 && save < array[j-1]; j--)
        array[j] = array[j-1];
        array[j] = save;
    }
}
#endif

class TSPoint {
    public:
    TSPoint(void) { x = y = 0; }
    TSPoint(int16_t x0, int16_t y0, int16_t z0) { x = x0; y = y0; z = z0; }
    bool operator==(TSPoint p1) { return ((p1.x == x) && (p1.y == y) && (p1.z == z)); }
    bool operator!=(TSPoint p1) { return ((p1.x != x) || (p1.y != y) || (p1.z != z)); }
    int16_t x, y, z;
};

class TouchScreen {
    public:
    TouchScreen() {
        _xp = {TS_XP_PORT, TS_XP_PIN, 0};
        _xm = {TS_XM_PORT, TS_XM_PIN, TS_XM_ADC_CHANNEL};
        _yp = {TS_YP_PORT, TS_YP_PIN, TS_YP_ADC_CHANNEL};
        _ym = {TS_YM_PORT, TS_YM_PIN, 0};
        _rxplate = 0;
        _adc = ADC1;
    }

    TouchScreen(uint16_t rxplate) {
        _xp = {TS_XP_PORT, TS_XP_PIN, 0};
        _xm = {TS_XM_PORT, TS_XM_PIN, TS_XM_ADC_CHANNEL};
        _yp = {TS_YP_PORT, TS_YP_PIN, TS_YP_ADC_CHANNEL};
        _ym = {TS_YM_PORT, TS_YM_PIN, 0};
        _rxplate = rxplate;
        _adc = ADC1;
    }

    TouchScreen(ADC_TypeDef* adc, uint16_t rxplate = 0) {
        _xp = {TS_XP_PORT, TS_XP_PIN, 0};
        _xm = {TS_XM_PORT, TS_XM_PIN, TS_XM_ADC_CHANNEL};
        _yp = {TS_YP_PORT, TS_YP_PIN, TS_YP_ADC_CHANNEL};
        _ym = {TS_YM_PORT, TS_YM_PIN, 0};
        _rxplate = rxplate;
        _adc = adc ? adc : ADC1;
    }

    TouchScreen(TS_Pin xp, TS_Pin yp, TS_Pin xm, TS_Pin ym, ADC_TypeDef* adc = ADC1, uint16_t rxplate = 0) {
        _xp = xp; _yp = yp; _xm = xm; _ym = ym;
        _rxplate = rxplate;
        _adc = adc ? adc : ADC1;
    }

    void setADC(ADC_TypeDef* adc) {
        _adc = adc ? adc : ADC1;
    }

    uint16_t readADC(uint32_t channel) {
        return ts_ll_analog_read(_adc, channel);
    }
    
    void pin_set(const TS_Pin& in1, const TS_Pin& in2, const TS_Pin& out1, const TS_Pin& out2, const TS_Pin& h, const TS_Pin& l) {
        ts_ll_pin_analog(in1.port, in1.pin);
        ts_ll_pin_analog(in2.port, in2.pin);
        ts_ll_pin_output(out1.port, out1.pin);
        ts_ll_pin_output(out2.port, out2.pin);
        ts_ll_digital_write(l.port, l.pin, 0);
        ts_ll_digital_write(h.port, h.pin, 1);
        ts_settle_delay();
    }
    
    TSPoint getPoint(void) {
        int x = 0, y = 0, z = 0;
        int samples[NUMSAMPLES];
        uint8_t i, valid = 1;

        // Z (pressure) read: XP=LOW, YM=HIGH, XM ve YP INPUT/ANALOG
        pin_set(_yp, _xm, _xp, _ym, _ym, _xp);
        int z1 = ts_ll_analog_read(_adc, _xm.channel);
        int z2 = ts_ll_analog_read(_adc, _yp.channel);
        z = (4095 - (z2 - z1));

        if (z > MINPRESSURE && z < MAXPRESSURE) {
            // X read: XP=HIGH, XM=LOW; YP ve YM INPUT/ANALOG
            pin_set(_ym, _yp, _xp, _xm, _xp, _xm);
            for (i = 0; i < NUMSAMPLES; i++) samples[i] = ts_ll_analog_read(_adc, _yp.channel);
            #if NUMSAMPLES > 2
                insert_sort(samples, NUMSAMPLES);
            #endif
            #if NUMSAMPLES == 2
                if (samples[0] < samples[1]-8 || samples[0] > samples[1]+8) valid = 0; 
            #endif
            x = (4095 - samples[NUMSAMPLES/2]);

            // Y read: YP=HIGH, YM=LOW; XP ve XM INPUT/ANALOG
            pin_set(_xp, _xm, _yp, _ym, _yp, _ym);
            for (i = 0; i < NUMSAMPLES; i++) samples[i] = ts_ll_analog_read(_adc, _xm.channel);
            #if NUMSAMPLES > 2
                insert_sort(samples, NUMSAMPLES);
            #endif
            #if NUMSAMPLES == 2
                if (samples[0] < samples[1]-8 || samples[0] > samples[1]+8) valid = 0;
            #endif
            y = (4095 - samples[NUMSAMPLES/2]);
        } else { z = 0; }

        if (!valid) z = 0;

        // LCD veri ve kontrol hatlarını tekrar OUTPUT moduna al (ekran çizimleri için)
        ts_ll_pin_output(_xm.port, _xm.pin);
        ts_ll_pin_output(_xp.port, _xp.pin);
        ts_ll_pin_output(_yp.port, _yp.pin);
        ts_ll_pin_output(_ym.port, _ym.pin);

        int16_t y1;
        switch(lcd.rotation) {
            case 0: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height); break;
            case 1: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                    y1 = y; y = lcd.Height-x; x = y1; break;
            case 2: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Width);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Height);  
                    x = lcd.Width-x; y = lcd.Height-y; break;
            case 3: x = MAP(x, TS_MINX, TS_MAXX, 0, lcd.Height);
                    y = MAP(y, TS_MINY, TS_MAXY, 0, lcd.Width);
                    y1 = y; y = x; x = lcd.Width-y1; break;
        }
        return TSPoint(x, y, z);
    }

    private:
    TS_Pin _yp, _ym, _xm, _xp;
    uint16_t _rxplate;
    ADC_TypeDef* _adc;
};

TouchScreen ts=TouchScreen(295);

#endif
