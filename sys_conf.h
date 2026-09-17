#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define OC_90MHz 90
#define OC_96MHz 96
#define OC_100MHz 100
#define OC_108MHz 108
#define OC_120MHz 120
#define OC_130MHz 130
#define OC_144MHz 144

void SystemClock_Cfg(int PLLN, uint32_t PLLP, int PLLQ, uint32_t AHB, uint32_t APB1, uint32_t APB2);

void SystemClock_OC(int OC) {
    #if defined(STM32F4xx)
    switch(OC) {
        case 90:  SystemClock_Cfg(180, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 96:  SystemClock_Cfg(192, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 100: SystemClock_Cfg(200, RCC_PLLP_DIV2, 4, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 108: SystemClock_Cfg(432, RCC_PLLP_DIV4, 9, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 120: SystemClock_Cfg(240, RCC_PLLP_DIV2, 5, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 130: SystemClock_Cfg(260, RCC_PLLP_DIV2, 5, RCC_SYSCLK_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1); break;
        case 144: SystemClock_Cfg(288, RCC_PLLP_DIV2, 6, RCC_SYSCLK_DIV1, RCC_HCLK_DIV4, RCC_HCLK_DIV2); break;
        default: Serial.println("Overclock could not be applied !");
    }
    #else
        #error "----- This function can be used for the STM32F401. -----"
    #endif
}

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
}

void SystemClock_Config84(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2); // sacal1=100mhz. scale2=84mhz
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler();	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)	{ Error_Handler(); }
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
}

#ifdef __cplusplus
}
#endif
