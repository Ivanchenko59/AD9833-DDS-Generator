/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "encoder.h"
#include "AD9833.h"
#include "st7735.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
Color_TypeDef text_color = {ST7735_WHITE, ST7735_BLACK, ST7735_BLACK, ST7735_YELLOW};
Selector_TypeDef Menu_Selector = Main_Menu;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int8_t encoder_status;
uint32_t freq = 1000;
int8_t edit_pos = 3;
uint16_t MHz, kHz, Hz;
char Str_Buffer[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();
  Encoder_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  ST7735_FillScreen(ST7735_BLACK);

  ST7735_FillRectangle(25, 70, 12, 2, ST7735_DARKGREY);
  ST7735_FillRectangle(49, 70, 35, 2, ST7735_DARKGREY);
  ST7735_FillRectangle(97, 70, 35, 2, ST7735_DARKGREY);
  ST7735_WriteString(21, 75, "MHz  kHz    Hz", Font_7x10, ST7735_DARKGREY, ST7735_BLACK);

  MHz = freq / 1000000;
  kHz = freq / 1000 % 1000;
  Hz = freq % 1000;
  sprintf(Str_Buffer, "%1d,%03d,%03d", MHz, kHz, Hz);
  SquareIcon(15, 95, ST7735_LIGHTGREY);
  TriangleIcon (55, 95, ST7735_LIGHTGREY);


  while (1)
  {


	  uint8_t button_status = Button_Get_Status();
	  switch(button_status) {
	  	  case Short_Press:
	  		  Menu_Selector++;
	  		  break;
	  	  case Long_Press:
	  		  Menu_Selector--;
	  		  break;
	  	  case False_Press:
	  		  break;
	  }


	  switch(Menu_Selector) {

		  case Main_Menu:
			  ST7735_WriteString(25, 50, Str_Buffer, Font_12x18, ST7735_WHITE, ST7735_BLACK);
			  break;

		  case Change_Wave_Form:

			  break;

		  case Change_Frequency:

			  Change_Position(&edit_pos);

			  MHz = freq / 1000000;
			  kHz = freq / 1000 % 1000;
			  Hz = freq % 1000;

			  sprintf(Str_Buffer, "%1d,%03d,%03d", MHz, kHz, Hz);
			  ST7735_WriteStringWithSelect(25, 50, Str_Buffer, Font_12x18, edit_pos, text_color);

			  //Edit_Frequency(edit_pos, &freq);
			  break;
		  case Sweep_Mode:

			  break;
		  default:
			  break;


	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}



void Edit_Frequency(int8_t position, uint32_t *p_freq)
{
	uint32_t pos_to_Hz = int_pow(10, position);
	uint32_t edit_value = *p_freq / pos_to_Hz;

	switch(Encoder_Get_Status()) {
		  case Incremented:
			  edit_value++;
			  break;
		  case Decremented:
			  if(edit_value > 0)
				  edit_value--;
			  break;
		  case Neutral:
			  break;
	  }

	uint32_t edited_freq = edit_value * pos_to_Hz + *p_freq % pos_to_Hz;

	if (edited_freq <= MAX_FREQ_VALUE)
		*p_freq = edited_freq;
}

void Change_Position(int8_t *edit_pos)
{
	switch(Encoder_Get_Status()) {
		case Incremented:
			(*edit_pos)--;
			if (*edit_pos < 0) *edit_pos = MAX_DIGITS;
			break;
		case Decremented:
			(*edit_pos)++;
			if (*edit_pos >= MAX_DIGITS + 1) *edit_pos = 0;
			break;
		case Neutral:
			break;
	}
}

uint32_t int_pow(uint32_t base, uint8_t exp)
{
    uint32_t result = 1;
    while (exp) {
        if (exp % 2)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
