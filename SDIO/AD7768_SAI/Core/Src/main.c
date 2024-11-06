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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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

SD_HandleTypeDef hsd1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char TxBuffer[250];
FIL Fil;
    FRESULT FR_Status,Open_Status;
    FATFS *FS_Ptr;
    UINT RWC, WWC; // Read/Write Word Counter
    DWORD FreeClusters;
    uint32_t TotalSize, FreeSpace;
    char RW_Buffer[250];
static void SDIO_SDCard_Test(void)
{
    

    do
    {
        //------------------[ Mount The SD Card ]--------------------
        FR_Status = f_mount(&SDFatFS,SDPath, 1);
        if (FR_Status != FR_OK)
        {
            sprintf(TxBuffer, "Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
            HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
            break;
        }
        sprintf(TxBuffer, "SD Card Mounted Successfully! \r\n\n");
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);

        //------------------[ Get & Print The SD Card Size & Free Space ]--------------------
        f_getfree("", &FreeClusters, &FS_Ptr);
        TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
        FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
        sprintf(TxBuffer, "Total SD Card Size: %u Bytes\r\n", TotalSize);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        sprintf(TxBuffer, "Free SD Card Space: %u Bytes\r\n\n", FreeSpace);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        //------------------[ Open A Text File For Write & Write Data ]--------------------
        //Open the file
        Open_Status = f_open(&Fil, "MyTextFile.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
        if(Open_Status != FR_OK)
        {
            sprintf(TxBuffer, "Error! While Creating/Opening A New Text File, Error Code: (%i)\r\n", Open_Status);
            HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
            break;
        }
        sprintf(TxBuffer, "Text File Created & Opened! Writing Data To The Text File..\r\n\n");
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        // (1) Write Data To The Text File [ Using f_puts() Function ]
        f_puts("Hello! From STM32 To SD Card Over SDMMC, Using f_puts()\n", &Fil);
        // (2) Write Data To The Text File [ Using f_write() Function ]
        strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SDMMC, Using f_write()\r\n");
        f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
        // Close The File
        f_close(&Fil);
        //------------------[ Open A Text File For Read & Read Its Data ]--------------------
        // Open The File
        Open_Status = f_open(&Fil, "MyTextFile.txt", FA_READ);
        if(FR_Status != FR_OK)
        {
            sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Read.. \r\n");
            HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
            break;
        }
        // (1) Read The Text File's Data [ Using f_gets() Function ]
        f_gets(RW_Buffer, sizeof(RW_Buffer), &Fil);
        sprintf(TxBuffer, "Data Read From (MyTextFile.txt) Using f_gets():%s", RW_Buffer);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        // (2) Read The Text File's Data [ Using f_read() Function ]
        f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
        sprintf(TxBuffer, "Data Read From (MyTextFile.txt) Using f_read():%s", RW_Buffer);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        // Close The File
        f_close(&Fil);
        sprintf(TxBuffer, "File Closed! \r\n\n");
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        //------------------[ Open An Existing Text File, Update Its Content, Read It Back ]--------------------
        // (1) Open The Existing File For Write (Update)
        Open_Status = f_open(&Fil, "MyTextFile.txt", FA_OPEN_EXISTING | FA_WRITE);
        FR_Status = f_lseek(&Fil, f_size(&Fil)); // Move The File Pointer To The EOF (End-Of-File)
        if(FR_Status != FR_OK)
        {
            sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Update.. \r\n");
            HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
            break;
        }
        // (2) Write New Line of Text Data To The File
        FR_Status = f_puts("This New Line Was Added During File Update!\r\n", &Fil);
        f_close(&Fil);
        memset(RW_Buffer,'\0',sizeof(RW_Buffer)); // Clear The Buffer
        // (3) Read The Contents of The Text File After The Update
        FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ); // Open The File For Read
        f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
        sprintf(TxBuffer, "Data Read From (MyTextFile.txt) After Update:\r\n%s", RW_Buffer);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        f_close(&Fil);
        //------------------[ Delete The Text File ]--------------------
        // Delete The File

        FR_Status = f_unlink("MyTextFile.txt");
        if (FR_Status != FR_OK) {
            sprintf(TxBuffer, "Error! While Deleting The (MyTextFile.txt) File.. \r\n");
            HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
        }

    } while(0);
    //------------------[ Test Complete! Unmount The SD Card ]--------------------
    FR_Status = f_mount(NULL, "", 0);
    if (FR_Status != FR_OK)
    {
        sprintf(TxBuffer, "\r\nError! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
    } else {
        sprintf(TxBuffer, "\r\nSD Card Un-mounted Successfully! \r\n");
        HAL_UART_Transmit(&huart3,(uint8_t *) TxBuffer,strlen(TxBuffer),HAL_MAX_DELAY);
    }
}
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
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(5000);
	SDIO_SDCard_Test();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
				
			


  /* USER CODE END 3 */
}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 25;
  RCC_OscInitStruct.PLL.PLLR = 8;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 8;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
