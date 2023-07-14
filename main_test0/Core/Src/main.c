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
#include <stdio.h>
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "rs485.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char Buff[255];//存储数据
uint8_t Data;	//暂存数据
int Cnt = 0;	//计数
 
/* 回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
			Buff[Cnt++] = (char)Data;
			if(Buff[Cnt-1] == 0x0A && Buff[Cnt-2] == 0x0D)//判断是否为回车结尾
			{
				printf("\n接收到的数据为：\n");
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)Buff, Cnt);//显示在串口助手
				HAL_UART_Transmit_IT(&huart3, (uint8_t *)Buff, Cnt);//发送给蓝牙
				Cnt = 0;
			}
		HAL_UART_Receive_IT(&huart1, &Data, sizeof(Data));//继续接收数据
	}
	if(huart ->Instance == USART2)
    {
        //将接收到的数据打印
        printf("RX Data[0]:0x%02x\r\n", Buff[0]);
        printf("RX Data[1]:0x%02x\r\n", Buff[1]);
        printf("RX Data[2]:0x%02x\r\n", Buff[2]);
        printf("RX Data[3]:0x%02x\r\n", Buff[3]);
        
        //重新使能串口接收中断
        HAL_UART_Receive_IT(&RS485_USART, &Data, 4);
    }
	if(huart == &huart3)
	{
			Buff[Cnt++] = (char)Data;
			if(Buff[Cnt-1] == 0x0A && Buff[Cnt-2] == 0x0D)//判断是否为回车结尾
			{
				printf("\n蓝牙返回的数据为：\n");
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)Buff, Cnt);//显示在串口助手
				Cnt = 0;
			}
		HAL_UART_Receive_IT(&huart3, &Data, sizeof(Data));//继续接收数据
	}
}
/* USER CODE END 0 */
/**
* @breif   初始化RS485收发器为接收模式
* @param   无
* @retval  无
* @note    在调用此函数之前，确保已经初始化通信串口和控制GPIO
*/
void RS485_Init()
{
    /* 使能串口接收中断 */
    HAL_UART_Receive_IT(&RS485_USART, &Data, 4);
    
    RS485_RX_ENABLE();
}
/**
* @breif   控制RS485收发器发送数据
* @param   data 要发送的数据
* @param   len  要发送数据的长度
* @retval  0 - 发送成功，-1 = 发送失败
*/
int RS485_Transmit(uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;
    
    RS485_TX_ENABLE();
    
    status = HAL_UART_Transmit(&RS485_USART, data, len, 0xFFFF);
    
    HAL_Delay(1);
    
    RS485_RX_ENABLE();
    
    return status == HAL_OK ? 0 : -1;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
uint8_t data[4];
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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
printf("-----RS485 1# Board ------\r\n");

RS485_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
HAL_UART_Receive_IT(&huart1, &Data, sizeof(Data));//开启中断接收数据
HAL_UART_Receive_IT(&huart3, &Data, sizeof(Data));//开启中断接收数据

  while (1)
  {
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
		RS485_Transmit(data,4);
		
		HAL_Delay(5000);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
