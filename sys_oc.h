#pragma once

    // This file applies to STM32F401 devices equipped with a 25MHz crystal and 
    // STM32F103 devices equipped with 8MHz crystal. 
    // Do not use it if you lack sufficient knowledge about your hardware.
    // Adverse results may occur. You bear full responsibility.

#ifdef __cplusplus
extern "C" {
#endif

#define OC_96MHz 96

#if defined(STM32F4xx)
void SystemClock_Cfg(int PLLN, uint32_t PLLP, int PLLQ, uint32_t AHB, uint32_t APB1, uint32_t APB2)  {
	RCC_OscInitTypeDef RCC_OscInitStruct = {};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
	RCC->CFGR &= ~RCC_CFGR_SW; // SYSCLK = HSI
	while ((RCC->CFGR & RCC_CFGR_SWS));
	RCC->CR &= ~RCC_CR_PLLON;
	while (RCC->CR & RCC_CR_PLLRDY);
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1); // sacal1=100mhz. scale2=84mhz
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;             // 25MHz / 25 = 1MHz
	RCC_OscInitStruct.PLL.PLLN = PLLN;           // 1MHz * 192 = 192MHz
	RCC_OscInitStruct.PLL.PLLP = PLLP;           // 192MHz / 2 = 96MHz
	RCC_OscInitStruct.PLL.PLLQ = PLLQ;           // 192MHz / 4 = 48Mhz   
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler();	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = AHB;
	RCC_ClkInitStruct.APB1CLKDivider = APB1;
	RCC_ClkInitStruct.APB2CLKDivider = APB2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)	{ Error_Handler(); }
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	// SYSCLK = PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	SystemCoreClockUpdate();
    HAL_InitTick(TICK_INT_PRIORITY);
}
#elif defined(STM32F1xx)
void SystemClock_Cfg (uint32_t PLLMULL, uint32_t PPRE1, uint32_t PPRE2, uint32_t ADCPRE) {
    // Switch to HSI first, then disable PLL.
    RCC->CFGR &= ~RCC_CFGR_SW;           // SYSCLK = HSI
    while ((RCC->CFGR & RCC_CFGR_SWS));  

    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);     

    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
    
    #if defined(FLASH_ACR_PRFTBE)
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    #endif

    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_2;
    // PLL + bus + ADC
    RCC->CFGR = RCC_CFGR_PLLSRC       |   // HSE → PLL
                PLLMULL |   // 8MHz × 12 = 96MHz
                PPRE1   |   // APB1 96/2 = 48MHz (limit 36MHz)
                PPRE2   |   // APB2 = 96MHz (TIM1 )
                ADCPRE;     // ADC  = 12MHz

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // SYSCLK = PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	SystemCoreClockUpdate();
    HAL_InitTick(TICK_INT_PRIORITY);
}
#endif

void SystemClock_OC(int OC) {
    #if defined(STM32F4xx)
    switch(OC) {
        //case 90:  SystemClock_Cfg(180, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 96:  SystemClock_Cfg(192, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        //case 100: SystemClock_Cfg(200, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        //case 108: SystemClock_Cfg(432, RCC_PLLP_DIV4, 9, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        //case 120: SystemClock_Cfg(240, RCC_PLLP_DIV2, 5, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        //case 130: SystemClock_Cfg(260, RCC_PLLP_DIV2, 5, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        //case 144: SystemClock_Cfg(288, RCC_PLLP_DIV2, 6, RCC_SYSCLK_DIV1, RCC_HCLK_DIV4, RCC_HCLK_DIV2); break;
        //default: Serial.println("Overclock could not be applied !"); break;
    }
    #elif defined(STM32F1xx)
    switch(OC) {
        case 96:  SystemClock_Cfg(RCC_CFGR_PLLMULL12, RCC_CFGR_PPRE1_DIV2, RCC_CFGR_PPRE2_DIV1, RCC_CFGR_ADCPRE_DIV8); break;
    }
    #endif


    #if defined(__cplusplus) && defined(ARDUINO)
        Serial.print(F("[SYS_OC] Target: "));
        Serial.print(OC);
        Serial.print(F(" MHz, Active System Time: "));
        Serial.print(SystemCoreClock / 1000000);
        Serial.println(F(" MHz"));
    #endif
}

#ifdef __cplusplus
}
#endif
