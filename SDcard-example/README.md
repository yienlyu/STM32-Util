# L4 FatFs Example to L496-DISCO: Create IOC file

## Example Source
`/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/`

## Project ioc Settings

### Connectivity
#### I2C2
- Mode: I2C
- Configuration
	- GPIO
		- PB14: I2C2_SDA
		- PH4: I2C2_SCL
	- DMA: add I2C2\_RX and I2C2\_TX

#### SDMMC1
- Mode: SD 4 bits Wide bus
- Configuration
	- GPIO
		- PC8: SDMMC1\_D0
		- PC9: SDMMC1\_D1
		- PC10: SDMMC1\_D2
		- PC11: SDMMC1\_D3
		- PC12: SDMMC1\_CK
		- PD2: SDMMC1\_CMD
	- DMA: add SDMMC1\_RX and SDMMC1\_TX

### Middleware
#### FATFS
- Mode: SD card
- Configuration
	- Platform
		- BSP: Detect\_SDIO
		Turn on PB13 as GPIO\_Input, then choose it as the found solution for BSP Detect\_SDIO

### System Core
Check DMA and GPIO are properly set with the above. 

### Clock Configuration
![](https://i.imgur.com/AUgowKg.png)

## Project Includes
Include these libraries to your project:
- `/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/Components/Common/`
- `/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L496G-Discovery/`
- `/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Device/STM32L4xx/Include/`
- `/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Include/`
- `/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L4xx_HAL_Driver/Inc/`
- `/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/`
- `/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/drivers/`
- `/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/Inc`

## Changes to the Code
### main.c
```
/* USER CODE BEGIN PFP */
static void FS_FileOperations(void);
static void SD_Initialize(void);

static uint8_t isInitialized = 0;
static uint8_t isCreated = 0;
uint8_t workBuffer[_MAX_SS];

/* USER CODE END PFP */
```

```
  /* USER CODE BEGIN 2 */
  // init MFX
  uint8_t ret_mfx = IO_ERROR;

  // init after gpio init
  ret_mfx = BSP_IO_Init();

  BSP_LED_Init(LED2);
  BSP_LED_Init(LED1);


  if (ret_mfx == IO_OK) {
	  BSP_LED_On(LED1);
	  BSP_LED_On(LED2);
  }
  	BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);

    SD_Initialize();

    if(BSP_SD_IsDetected())
    {
      Appli_state = APPLICATION_RUNNING;
    }
  /* USER CODE END 2 */
```

```
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	switch(Appli_state)
	{
	case APPLICATION_RUNNING:
	  BSP_LED_Off(LED1);
	  SD_Initialize();
	  FS_FileOperations();
	  Appli_state = APPLICATION_IDLE;
	  break;

	case APPLICATION_IDLE:
	  break;

	case APPLICATION_SD_UNPLUGGED:
	  if (isInitialized == 1)
	  {
		Error_Handler();
		isInitialized = 0;
	  }

	  Appli_state = APPLICATION_IDLE;
	  break;

	default:
	  break;
	}
    /* USER CODE END WHILE */
```

```
/* USER CODE BEGIN 4 */
static void FS_FileOperations(void)
{
  FRESULT res; /* FatFs function common result code */
  uint32_t byteswritten, bytesread; /* File write/read counts */
  uint8_t wtext[] = "stm32l496g_discovery : This is STM32 working with FatFs and uSD diskio driver"; /* File write buffer */
  uint8_t rtext[100]; /* File read buffer */

  /* Register the file system object to the FatFs module */
  if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
  {
#if FATFS_MKFS_ALLOWED
    if (isCreated == 0)
    {
      res = f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer));

      if (res != FR_OK)
      {
        Error_Handler();
        while(1);
      }
    }
    isCreated = 1;
#endif
    /* Create and Open a new text file object with write access */
    if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    {
      /* Write data to the text file */
      res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

      if((byteswritten > 0) && (res == FR_OK))
      {
        /* Close the open text file */
        f_close(&MyFile);

        /* Open the text file object with read access */
        if(f_open(&MyFile, "STM32.TXT", FA_READ) == FR_OK)
        {
          /* Read data from the text file */
          res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

          if((bytesread > 0) && (res == FR_OK))
          {
            /* Close the open text file */
            f_close(&MyFile);

            /* Compare read data with the expected data */
            if((bytesread == byteswritten))
            {
              /* Success of the demo: no error occurrence */
              BSP_LED_On(LED2);
              return;
            }
          }
        }
      }
    }
  }
  /* Error */
  Error_Handler();
}

static void SD_Initialize(void)
{
  if (isInitialized == 0)
  {
    if (BSP_SD_Init() == MSD_OK)
    {
      BSP_SD_ITConfig();
      isInitialized = 1;
    }
    else
    {
      BSP_LED_On(LED1);
      /* wait until the uSD is plugged */
      while (BSP_SD_IsDetected() != SD_PRESENT)
      {}
      BSP_LED_Off(LED1);
    }
  }
}
/* USER CODE END 4 */
```

### Differences and Notifications
- You should do `BSP_IO_Init()` after `MX_I2C2_Init()`, since MFX has to communicate through I2C.
- You don't have to do `FATFS_LinkDriver(&SD_Driver, SDPath)` again, since it has already been done in `MX_FATFS_Init()`.
- No need to turn on "Use dma template" in FATFS Configurations.
