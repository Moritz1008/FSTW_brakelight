/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t	brake_pressure_front = 0;
uint16_t rx_timer = 0;
uint8_t bl_state = 0;
uint8_t inverterTemp = 0;
uint8_t pump_state = 0;

uint8_t button = 0;
uint8_t buttonstate = 0;
uint8_t button_old = 0;

uint32_t fan_timer = 0;

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
  MX_CAN_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  FAN_TIMER->FAN_CHANNEL = 19000-1;
  fan_timer = HAL_GetTick();

  HAL_Delay(1000);
  for (int i=0; i<3; i++){
	  HAL_GPIO_WritePin(BRAKELIGHT_LED, GPIO_PIN_SET);
	  HAL_Delay(60);
	  HAL_GPIO_WritePin(BRAKELIGHT_LED, GPIO_PIN_RESET);
	  HAL_Delay(100);
	  HAL_GPIO_WritePin(BRAKELIGHT_LED, GPIO_PIN_SET);
	  HAL_Delay(60);
	  HAL_GPIO_WritePin(BRAKELIGHT_LED, GPIO_PIN_RESET);
	  HAL_Delay(400);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_Delay(1);

    // Handle Brakelight
	  if (brake_pressure_front > BP_THRESHOLD) {
		  bl_state = 1;
	  }
	  if (brake_pressure_front < BP_THRESHOLD / 2){
		  bl_state = 0;
	  }

	  HAL_GPIO_WritePin(BRAKELIGHT_LED, bl_state);

	  if (rx_timer > 1000) {
		  brake_pressure_front = 0;
	  }
	  rx_timer++;

    // Handle Pump
	  if (inverterTemp > PUMP_THRESHOLD) {
		  pump_state = 1;
	  }
	  if (inverterTemp < PUMP_THRESHOLD - 10){
		  pump_state = 0;
	  }
    HAL_GPIO_WritePin(PUMP, pump_state);

    // Handle Fan
    if (HAL_GetTick() - fan_timer > 6000) {
      if (pump_state) {
        FAN_TIMER->FAN_CHANNEL = 18945;
      } else {
        FAN_TIMER->FAN_CHANNEL = 20000-1;
      }
    }
    // if (button != button_old) {
    //     if (button == 1) {
    //         buttonstate = !buttonstate;
    //     }
    //     button_old = button;
    // }



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
