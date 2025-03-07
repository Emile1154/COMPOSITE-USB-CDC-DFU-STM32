/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "usb_device.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "usbd_desc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef  void (*pFunction)(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
// static void MX_RTC_Init(void);
// static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t AddressMyApplicationBegin  = 0x0800C000; 
uint32_t AddressMyApplicationEnd = 0x0800FBFC;
pFunction JumpToApplication;
uint32_t JumpAddress;
uint8_t usb_mode = 0;

uint16_t usb_pid_value;
char *product_str_fs;
char *conf_str_fs;
char *interface_str_fs;
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
  //MX_RTC_Init();
  //MX_TIM2_Init();
  
  /* USER CODE BEGIN 2 */
  //HAL_TIM_Base_Start(&htim2);
  //init usb in DFU mode
  if(HAL_GPIO_ReadPin(boot1_GPIO_Port, boot1_Pin ) == GPIO_PIN_SET) 
  {
      /* Test if user code is programmed starting from address 0x0800C000 */
      if (((*(__IO uint32_t *) USBD_DFU_APP_DEFAULT_ADD) & 0x2FFE0000) == 0x20000000)
      {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t *) (USBD_DFU_APP_DEFAULT_ADD + 4);
        JumpToApplication = (pFunction) JumpAddress;

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *) USBD_DFU_APP_DEFAULT_ADD);
        JumpToApplication();
      }
      
      usb_pid_value = 57105;

      product_str_fs = (char*) malloc(sizeof(char)*31);
      product_str_fs = "STM32 DownLoad Firmware Update";

      conf_str_fs = (char*) malloc(sizeof(char)*11);
      conf_str_fs = "DFU Config";

      interface_str_fs = (char*) malloc(sizeof(char)*14);
      interface_str_fs = "DFU Interface";

      usb_mode = 0;
      
  }else{  //init usb in CDC mode
      product_str_fs = (char*) malloc(sizeof(char)*23);
      product_str_fs = "STM32 Virtual COM Port";

      conf_str_fs = (char*) malloc(sizeof(char)*11);
      conf_str_fs = "CDC Config";

      interface_str_fs = (char*) malloc(sizeof(char)*14);
      interface_str_fs = "CDC Interface";

      usb_pid_value = 22336;
      usb_mode = 1;
  }

  update_usb_descriptor(usb_pid_value);
  MX_USB_DEVICE_Init();
  uint8_t msg[] = "test logs!\r\n";
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    if(usb_mode == 1){
      CDC_Transmit_FS(msg, sizeof(msg));
      HAL_Delay(1000);
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

// /**
//   * @brief RTC Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_RTC_Init(void)
// {

//   /* USER CODE BEGIN RTC_Init 0 */

//   /* USER CODE END RTC_Init 0 */

//   RTC_TimeTypeDef sTime = {0};
//   RTC_DateTypeDef DateToUpdate = {0};

//   /* USER CODE BEGIN RTC_Init 1 */

//   /* USER CODE END RTC_Init 1 */

//   /** Initialize RTC Only
//   */
//   hrtc.Instance = RTC;
//   hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
//   hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
//   if (HAL_RTC_Init(&hrtc) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   /* USER CODE BEGIN Check_RTC_BKUP */

//   /* USER CODE END Check_RTC_BKUP */

//   /** Initialize RTC and set the Time and Date
//   */
//   sTime.Hours = 0x0;
//   sTime.Minutes = 0x0;
//   sTime.Seconds = 0x0;

//   if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
//   DateToUpdate.Month = RTC_MONTH_JANUARY;
//   DateToUpdate.Date = 0x1;
//   DateToUpdate.Year = 0x0;

//   if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN RTC_Init 2 */

//   /* USER CODE END RTC_Init 2 */

// }

// /**
//   * @brief TIM2 Initialization Function
//   * @param None
//   * @retval None
//   */
// static void MX_TIM2_Init(void)
// {

//   /* USER CODE BEGIN TIM2_Init 0 */

//   /* USER CODE END TIM2_Init 0 */

//   TIM_MasterConfigTypeDef sMasterConfig = {0};
//   TIM_OC_InitTypeDef sConfigOC = {0};

//   /* USER CODE BEGIN TIM2_Init 1 */

//   /* USER CODE END TIM2_Init 1 */
//   htim2.Instance = TIM2;
//   htim2.Init.Prescaler = 71;
//   htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//   htim2.Init.Period = 0;
//   htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//   htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//   if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
//   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//   if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   sConfigOC.OCMode = TIM_OCMODE_TIMING;
//   sConfigOC.Pulse = 0;
//   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//   if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /* USER CODE BEGIN TIM2_Init 2 */

//   /* USER CODE END TIM2_Init 2 */

// }

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : boot1_Pin */
  GPIO_InitStruct.Pin = boot1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(boot1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
