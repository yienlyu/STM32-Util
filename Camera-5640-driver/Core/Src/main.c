/**
  ******************************************************************************
  * @file    DCMI/DCMI_CaptureMode/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describe how to configure the camera in continuous mode
  *          and QVGA resolution.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup DCMI_CaptureMode
  * @{
  */

/* from H7 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VSYNC           1
#define VBP             1
#define VFP             1
#define VACT            480
#define HSYNC           1
#define HBP             1
#define HFP             1
#define HACT            400

#define LEFT_AREA         1
#define RIGHT_AREA        2

#define CAMERA_STATE_RESET               0
#define CAMERA_STATE_CAPTURE_ONGOING     1
#define CAMERA_STATE_DISPLAY_ONGOING     2

/* Private macro -------------------------------------------------------------*/
#define __DSI_MASK_TE()   (GPIOJ->AFR[0] &= (0xFFFFF0FFU))   /* Mask DSI TearingEffect Pin*/
#define __DSI_UNMASK_TE() (GPIOJ->AFR[0] |= ((uint32_t)(GPIO_AF13_DSI) << 8)) /* UnMask DSI TearingEffect Pin*/
/* Private variables ---------------------------------------------------------*/
uint8_t __IO cameraState = CAMERA_STATE_RESET;
//static uint32_t   LcdResX    = 0;
//static uint32_t   LcdResY    = 0;

/* Camera resolutions */
uint32_t CameraResolution[4] = {CAMERA_R160x120, CAMERA_R320x240, CAMERA_R480x272, CAMERA_R640x480};
uint32_t CameraResX[4] = {160, 320, 480, 640};
uint32_t CameraResY[4] = {120, 240, 272, 480};
uint32_t CameraResIndex = 0;

//extern LTDC_HandleTypeDef hlcd_ltdc;
//extern DSI_HandleTypeDef hlcd_dsi;

static __IO int32_t pending_buffer = -1;
static __IO int32_t active_area = 0;
static __IO int32_t dma2d_pending_copy = 0;

int32_t LCD_GetXSize(uint32_t Instance, uint32_t *XSize);
int32_t LCD_GetYSize(uint32_t Instance, uint32_t *YSize);

//const LCD_UTILS_Drv_t LCD_UTIL_Driver =
//{
//  BSP_LCD_DrawBitmap,
//  BSP_LCD_FillRGBRect,
//  BSP_LCD_DrawHLine,
//  BSP_LCD_DrawVLine,
//  BSP_LCD_FillRect,
//  BSP_LCD_ReadPixel,
//  BSP_LCD_WritePixel,
//  LCD_GetXSize,
//  LCD_GetYSize,
//  BSP_LCD_SetActiveLayer,
//  BSP_LCD_GetPixelFormat
//};
//
//DSI_CmdCfgTypeDef CmdCfg;
//DSI_LPCmdTypeDef LPCmd;
//DSI_PLLInitTypeDef dsiPllInit;
//static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

uint8_t pColLeft[]    = {0x00, 0x00, 0x01, 0x8F}; /*   0 -> 399 */
uint8_t pColRight[]   = {0x01, 0x90, 0x03, 0x1F}; /* 400 -> 799 */
uint8_t pPage[]       = {0x00, 0x00, 0x01, 0xDF}; /*   0 -> 479 */
uint8_t pScanCol[]    = {0x02, 0x15};             /* Scan @ 533 */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
//static void DMA2D_ConvertFrameToARGB8888(void *pSrc, void *pDst, uint32_t xsize, uint32_t ysize);
//static void DMA2D_Config(uint16_t xsize, uint32_t ysize);
//static void DMA2D_TransferCompleteCallback(DMA2D_HandleTypeDef *hdma2d);
//static int32_t DSI_IO_Write(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size);
//static int32_t DSI_IO_Read(uint16_t ChannelNbr, uint16_t Reg, uint8_t *pData, uint16_t Size);
//static uint8_t LCD_Init(void);
//static void LCD_LayertInit(uint16_t LayerIndex, uint32_t Address);
//static void LTDC_Init(void);
//static void Display_StartRefresh(void);
//static void Display_WaitRefresh(void);
//static void CPU_CACHE_Enable(void);
//static void MPU_Config(void);
//static void LCD_MspInit(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_I2C2_Init(void);
static void MX_FMC_Init(void);
void Error_Handler(void);

/* from L4 */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_rx;
DMA_HandleTypeDef hdma_i2c2_tx;

SD_HandleTypeDef hsd1;
DMA_HandleTypeDef hdma_sdmmc1_rx;
DMA_HandleTypeDef hdma_sdmmc1_tx;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

SRAM_HandleTypeDef hsram1;


SRAM_HandleTypeDef hsram1;


extern DCMI_HandleTypeDef hDcmiHandler;
DMA2D_HandleTypeDef Dma2dHandle;
uint16_t pBuffer[ST7789H2_LCD_PIXEL_WIDTH * ST7789H2_LCD_PIXEL_WIDTH];
HAL_StatusTypeDef hal_status = HAL_OK;

/* Private function prototypes -----------------------------------------------*/
static void IO_Init(void);
static void LCD_ImagePreparation(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize);
static HAL_StatusTypeDef LCD_Write(uint32_t OrigAddress, uint32_t DestAddress, uint32_t TransferSize);
static void OnError_Handler(uint32_t condition);
static void DMA2D_Config(void);
static void TransferError(DMA2D_HandleTypeDef* Dma2dHandle);
static void TransferComplete(DMA2D_HandleTypeDef* Dma2dHandle);

/* Exported functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
//  HAL_StatusTypeDef hal_status = HAL_OK;

  /* STM32L4xx HAL library initialization:
   - Configure the Flash prefetch
   - Systick timer is configured by default as source of time base, but user
	 can eventually implement his proper time base source (a general purpose
	 timer for example or other time source), keeping in mind that Time base
	 duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	 handled in milliseconds basis.
   - Set NVIC Group Priority to 4
   - Low Level Initialization
 */
  HAL_Init();

  /* Configure the system clock to 80 MHz */
  SystemClock_Config();


  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDMMC1_SD_Init();
  MX_I2C2_Init();
  MX_FMC_Init();


  /*##-2- Initialize the LCD #################################################*/
  /*##-1- LEDs initialization  #################################################*/
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED1);


  /*##-2- LCD configuration  #################################################*/
  /* I/O initialization, required before LCD initialization */
  IO_Init();
  /* LCD initialization */
  BSP_LCD_Init();

  /* DMA2D initialization */
  DMA2D_Config();


  /*##-3- Camera Initialization ############################*/
  /* Initialize the Camera in QVGA mode */
//  BSP_CAMERA_Init(CAMERA_R320x240);
  /* Wait 1s to let auto-loops in the camera module converge and lead to correct exposure */
//  HAL_Delay(1000);
//
//  CAMERA_5640_Init(0,CameraResolution[CameraResIndex],CAMERA_PF_RGB565);
//  HAL_Delay(1000);

  /*##-4- Camera Continuous capture start in QVGA resolution ############################*/
  /* Disable unwanted HSYNC (IT_LINE)/VSYNC interrupts */
  __HAL_DCMI_DISABLE_IT(&hDcmiHandler, DCMI_IT_LINE | DCMI_IT_VSYNC);

  /* LCD size is 240 x 240 and format is RGB565 i.e. 16 bpp or 2 bytes/pixel.
     The LCD frame size is therefore 240 * 240 half-words of (240*240)/2 32-bit long words .
     Since the DMA associated to DCMI IP is configured in  BSP_CAMERA_MspInit() of stm32l496g_discovery_camera.c file
     with words alignment, the last parameter of HAL_DCMI_Start_DMA is set to:
      (ST7789H2_LCD_PIXEL_WIDTH*ST7789H2_LCD_PIXEL_HEIGHT)/2, that is 240 * 240 / 2
   */
//  hal_status = HAL_DCMI_Start_DMA(&hDcmiHandler, DCMI_MODE_CONTINUOUS,  (uint32_t)pBuffer , (ST7789H2_LCD_PIXEL_WIDTH*ST7789H2_LCD_PIXEL_HEIGHT)/2 );
//  OnError_Handler(hal_status != HAL_OK);

  /* Initialize the Camera */
  if(CAMERA_5640_Init(0,CameraResolution[CameraResIndex],CAMERA_PF_RGB565) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Start the Camera Capture */
  if(BSP_CAMERA_Start(0,(uint8_t *)CAMERA_FRAME_BUFFER,CAMERA_MODE_CONTINUOUS)!= BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  while (1) {
	// OK?!
  }
}

/**
  * @brief  Camera Frame Event callback.
  */
void BSP_CAMERA_FrameEventCallback(void)
{
  /* Fully fill the LCD screen */
  LCD_ImagePreparation(0, 0, ST7789H2_LCD_PIXEL_WIDTH, ST7789H2_LCD_PIXEL_HEIGHT);

  /* Write data (through DMA2D) */
  hal_status = LCD_Write((uint32_t) (&pBuffer), (uint32_t)&(LCD_ADDR->REG), ST7789H2_LCD_PIXEL_WIDTH * ST7789H2_LCD_PIXEL_HEIGHT);
  OnError_Handler(hal_status != HAL_OK);
}


/**
  * @brief IO initialization.
  * @note  GPIO PH.00 setting to activate STM32L496 Discovery I/Os
  *        and I/O initialization.
  * @retval None
  */
static void IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;


  __HAL_RCC_GPIOH_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull =   GPIO_NOPULL;
  GPIO_InitStruct.Alternate = 0;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

  HAL_GPIO_Init( GPIOH, &GPIO_InitStruct );

  HAL_GPIO_WritePin( GPIOH, GPIO_PIN_0, GPIO_PIN_RESET);

  /* Initialize the IO functionalities */
  BSP_IO_Init();
}

/**
  * @brief LCD image preparation.
  * @note  Configure image position and size for Discovery STM32L496 LCD
  *        and set LCD in pixel writing mode.
  *        This API must be followed by LCD_Write() call.
  * @param  x0: first pixel x position
  * @param  y0: first pixel y position
  * @param  xSize: image width (in pixels)
  * @param  ySize: image height (in pixels)
  * @retval None
  */
static void LCD_ImagePreparation(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize)
{
  /* CASET: Column Address Set */
  LCD_IO_WriteReg(ST7789H2_CASET);
  /* Send commands */
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(x0);
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(x0 + xSize -1);
  /* RASET: Row Address Set */
  LCD_IO_WriteReg(ST7789H2_RASET);
  /* Send commands */
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(y0);
  LCD_IO_WriteData(0x00);
  LCD_IO_WriteData(y0 + ySize -1);

  /* Prepare to write to LCD RAM */
  LCD_IO_WriteReg(ST7789H2_WRITE_RAM);
}

/**
  * @brief LCD image write-up.
  * @note  Resort to DMA to feed image pixels to STM32L496 Discovery LCD.
  *        LCD is configured in RGB565: one pixel is coded over 16 bits.
  * @param  OrigAddress: Image address
  * @param  DestAddress: LCD RAM address
  * @param  TransferSize: image size (in pixels)
  * @retval HAL status
  */
static HAL_StatusTypeDef LCD_Write(uint32_t OrigAddress, uint32_t DestAddress, uint32_t TransferSize)
{
  /* Force 1 pixel per line and width in pixels x height in pixels   */
  /* as number of lines to align DMA2D transfer to LCD configuration */
  if (HAL_OK != HAL_DMA2D_Start_IT(&Dma2dHandle, OrigAddress, DestAddress, 1, TransferSize))
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

static void DMA2D_Config(void)
{
  HAL_StatusTypeDef hal_status = HAL_OK;

  /* Enable DMA2D clock */
  __HAL_RCC_DMA2D_CLK_ENABLE();

  /* NVIC configuration for DMA2D transfer complete interrupt */
  HAL_NVIC_SetPriority(DMA2D_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);

  /* Configure the DMA2D Mode, Color Mode and output offset */
  Dma2dHandle.Init.Mode         = DMA2D_M2M;              /* DMA2D Mode memory to memory */
  Dma2dHandle.Init.ColorMode    = DMA2D_OUTPUT_RGB565;    /* Output color mode is RGB565 : 16 bpp */
  Dma2dHandle.Init.OutputOffset = 0x0;                    /* No offset in output */
  Dma2dHandle.Init.RedBlueSwap   = DMA2D_RB_REGULAR;      /* No R&B swap for the output image */
  Dma2dHandle.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;   /* No alpha inversion for the output image */

  /* DMA2D Callbacks Configuration */
  Dma2dHandle.XferCpltCallback  = TransferComplete;
  Dma2dHandle.XferErrorCallback = TransferError;

  /* Foreground Configuration : Layer 1 */
  Dma2dHandle.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  Dma2dHandle.LayerCfg[1].InputAlpha = 0xFF;                    /* Fully opaque */
  Dma2dHandle.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;  /* Foreground layer format is RGB565 : 16 bpp */
  Dma2dHandle.LayerCfg[1].InputOffset = 0x0;                    /* No offset in input */
  Dma2dHandle.LayerCfg[1].RedBlueSwap   = DMA2D_RB_REGULAR;     /* No R&B swap for the input foreground image */
  Dma2dHandle.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No alpha inversion for the input foreground image */

  Dma2dHandle.Instance = DMA2D;

  /* DMA2D Initialization */
  hal_status = HAL_DMA2D_Init(&Dma2dHandle);
  OnError_Handler(hal_status != HAL_OK);

  hal_status = HAL_DMA2D_ConfigLayer(&Dma2dHandle, 1);
  OnError_Handler(hal_status != HAL_OK);
}

/**
  * @brief  On Error Handler on condition TRUE.
  * @param  condition : Can be TRUE or FALSE
  * @retval None
  */
static void OnError_Handler(uint32_t condition)
{
  if(condition)
  {
    BSP_LED_On(LED1);
    while(1) { ; } /* Blocking on error */
  }
}

void Error_Handler(void)
{
  /* Turn LED REDon */
  BSP_LED_On(LED2);
  while(1)
  {
  }
}


/**
  * @brief  DMA2D Transfer complete callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
static void TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{
  /* Set LED2 On */
  BSP_LED_On(LED2);
}

/**
  * @brief  DMA2D error callback
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
static void TransferError(DMA2D_HandleTypeDef *hdma2d)
{
  /* Set LED1 On */
  BSP_LED_On(LED1);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
	Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
	Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin : DCMI_PWR_EN */
  GPIO_InitStruct.Pin = DCMI_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DCMI_PWR_EN_GPIO_Port, &GPIO_InitStruct);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA2_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel4_IRQn);
  /* DMA2_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

}


/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00000E14;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FMC_NORSRAM_DEVICE;
  hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_8;
  hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_DISABLE;
  hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_ENABLE;
  hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
  hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
