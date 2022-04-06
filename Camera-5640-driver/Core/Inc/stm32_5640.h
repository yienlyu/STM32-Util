#include "stm32l496g_discovery.h"
#include "stm32l496g_discovery_camera.h"
#include "camera.h"
/** @defgroup STM32H747I_DISCO_CAMERA_Exported_Constants Exported Constants
  * @{
  */
/* Camera instance number */
/* H7 discovery_camera.h */
#ifndef USE_CAMERA_SENSOR_OV9655
#define USE_CAMERA_SENSOR_OV9655         1
#endif

#ifndef USE_CAMERA_SENSOR_OV5640
#define USE_CAMERA_SENSOR_OV5640         1
#endif

#if (USE_CAMERA_SENSOR_OV9655 == 1)
#include "ov9655.h"
#endif
#if (USE_CAMERA_SENSOR_OV5640 == 1)
#include "ov5640.h"
#endif

#include "camera.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H747I_DISCO
  * @{
  */

/** @defgroup STM32H747I_DISCO_CAMERA CAMERA
  * @{
  */

/** @defgroup STM32H747I_DISCO_CAMERA_Exported_Types Exported Types
  * @{
  */
typedef struct
{
  uint32_t CameraId;
  uint32_t Resolution;
  uint32_t PixelFormat;
  uint32_t LightMode;
  uint32_t ColorEffect;
  int32_t  Brightness;
  int32_t  Saturation;
  int32_t  Contrast;
  int32_t  HueDegree;
  uint32_t MirrorFlip;
  uint32_t Zoom;
  uint32_t NightMode;
  uint32_t IsMspCallbacksValid;
}CAMERA_Ctx_t;

typedef struct
{
  uint32_t Resolution;
  uint32_t LightMode;
  uint32_t ColorEffect;
  uint32_t Brightness;
  uint32_t Saturation;
  uint32_t Contrast;
  uint32_t HueDegree;
  uint32_t MirrorFlip;
  uint32_t Zoom;
  uint32_t NightMode;
} CAMERA_Capabilities_t;

#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(DCMI_HandleTypeDef *);
  void (* pMspDeInitCb)(DCMI_HandleTypeDef *);
}BSP_CAMERA_Cb_t;
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_CAMERA_Exported_Constants Exported Constants
  * @{
  */
/* Camera instance number */
#define CAMERA_INSTANCES_NBR           1U

#define CAMERA_MODE_CONTINUOUS         DCMI_MODE_CONTINUOUS
#define CAMERA_MODE_SNAPSHOT           DCMI_MODE_SNAPSHOT

/* Camera resolutions */
//#define CAMERA_R160x120                 0U     /* QQVGA Resolution            */
//#define CAMERA_R320x240                 1U     /* QVGA Resolution             */
//#define CAMERA_R480x272                 2U     /* 480x272 Resolution          */
//#define CAMERA_R640x480                 3U     /* VGA Resolution              */
#define CAMERA_R800x480                 4U     /* WVGA Resolution             */

/* Camera Pixel Format */
#define CAMERA_PF_RGB565                0U     /* Pixel Format RGB565         */
#define CAMERA_PF_RGB888                1U     /* Pixel Format RGB888         */
#define CAMERA_PF_YUV422                2U     /* Pixel Format YUV422         */

/* Brightness */
#define CAMERA_BRIGHTNESS_MIN          -4
#define CAMERA_BRIGHTNESS_MAX           4

/* Saturation */
#define CAMERA_SATURATION_MIN          -4
#define CAMERA_SATURATION_MAX           4

/* Contrast */
#define CAMERA_CONTRAST_MIN            -4
#define CAMERA_CONTRAST_MAX             4

/* Hue Control */
#define CAMERA_HUEDEGREE_MIN           -6
#define CAMERA_HUEDEGREE_MAX            5

/* Mirror/Flip */
#define CAMERA_MIRRORFLIP_NONE          0x00U   /* Set camera normal mode     */
#define CAMERA_MIRRORFLIP_FLIP          0x01U   /* Set camera flip config     */
#define CAMERA_MIRRORFLIP_MIRROR        0x02U   /* Set camera mirror config   */

/* Zoom */
#define CAMERA_ZOOM_x8                  0x00U   /* Set zoom to x8             */
#define CAMERA_ZOOM_x4                  0x11U   /* Set zoom to x4             */
#define CAMERA_ZOOM_x2                  0x22U   /* Set zoom to x2             */
#define CAMERA_ZOOM_x1                  0x44U   /* Set zoom to x1             */

/* Color Effect */
//#define CAMERA_COLOR_EFFECT_NONE        0x00U   /* No effect                  */
//#define CAMERA_COLOR_EFFECT_BLUE        0x01U   /* Blue effect                */
//#define CAMERA_COLOR_EFFECT_RED         0x02U   /* Red effect                 */
//#define CAMERA_COLOR_EFFECT_GREEN       0x04U   /* Green effect               */
#define CAMERA_COLOR_EFFECT_BW          0x08U   /* Black and White effect     */
#define CAMERA_COLOR_EFFECT_SEPIA       0x10U   /* Sepia effect               */
#define CAMERA_COLOR_EFFECT_NEGATIVE    0x20U   /* Negative effect            */

/* Light Mode */
#define CAMERA_LIGHT_AUTO               0x00U   /* Light Mode Auto            */
#define CAMERA_LIGHT_SUNNY              0x01U   /* Light Mode Sunny           */
#define CAMERA_LIGHT_OFFICE             0x02U   /* Light Mode Office          */
#define CAMERA_LIGHT_HOME               0x04U   /* Light Mode Home            */
#define CAMERA_LIGHT_CLOUDY             0x08U   /* Light Mode Claudy          */

/* Night Mode */
#define CAMERA_NIGHT_MODE_SET           0x00U   /* Disable night mode         */
#define CAMERA_NIGHT_MODE_RESET         0x01U   /* Enable night mode          */

#define CAMERA_IRQHandler               DCMI_IRQHandler
#define CAMERA_DMA_IRQHandler           DMA2_Stream3_IRQHandler


#define CAMERA_OV9655_ADDRESS           0x60U
#define CAMERA_OV5640_ADDRESS           0x78U

/**
  * @}
  */

/** @addtogroup STM32H747I_DISCO_CAMERA_Exported_Variables
  * @{
  */
extern void                *Camera_CompObj;
extern DCMI_HandleTypeDef  hcamera_dcmi;
extern CAMERA_Ctx_t        Camera_Ctx[];
/**
  * @}
  */

/** @addtogroup STM32H747I_DISCO_CAMERA_Exported_Functions
  * @{
  */
//int32_t BSP_CAMERA_Init(uint32_t Instance, uint32_t Resolution, uint32_t PixelFormat);
//int32_t BSP_CAMERA_DeInit(uint32_t Instance);
#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
int32_t BSP_CAMERA_RegisterDefaultMspCallbacks (uint32_t Instance);
int32_t BSP_CAMERA_RegisterMspCallbacks(uint32_t Instance, BSP_CAMERA_Cb_t *CallBacks);
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 1) */
int32_t BSP_CAMERA_Start(uint32_t Instance, uint8_t *buff, uint32_t Mode);
//int32_t BSP_CAMERA_Stop(uint32_t Instance);
//int32_t BSP_CAMERA_Suspend(uint32_t Instance);
//int32_t BSP_CAMERA_Resume(uint32_t Instance);
int32_t BSP_CAMERA_GetCapabilities(uint32_t Instance, CAMERA_Capabilities_t *Capabilities);

int32_t BSP_CAMERA_SetResolution(uint32_t Instance, uint32_t Resolution);
int32_t BSP_CAMERA_GetResolution(uint32_t Instance, uint32_t *Resolution);

int32_t BSP_CAMERA_SetPixelFormat(uint32_t Instance, uint32_t PixelFormat);
int32_t BSP_CAMERA_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat);

int32_t BSP_CAMERA_SetLightMode(uint32_t Instance, uint32_t LightMode);
int32_t BSP_CAMERA_GetLightMode(uint32_t Instance, uint32_t *LightMode);

int32_t BSP_CAMERA_SetColorEffect(uint32_t Instance, uint32_t ColorEffect);
int32_t BSP_CAMERA_GetColorEffect(uint32_t Instance, uint32_t *ColorEffect);

int32_t BSP_CAMERA_SetBrightness(uint32_t Instance, int32_t Brightness);
int32_t BSP_CAMERA_GetBrightness(uint32_t Instance, int32_t *Brightness);

int32_t BSP_CAMERA_SetSaturation(uint32_t Instance, int32_t Saturation);
int32_t BSP_CAMERA_GetSaturation(uint32_t Instance, int32_t *Saturation);

int32_t BSP_CAMERA_SetContrast(uint32_t Instance, int32_t Contrast);
int32_t BSP_CAMERA_GetContrast(uint32_t Instance, int32_t *Contrast);

int32_t BSP_CAMERA_SetHueDegree(uint32_t Instance, int32_t HueDegree);
int32_t BSP_CAMERA_GetHueDegree(uint32_t Instance, int32_t *HueDegree);

int32_t BSP_CAMERA_SetMirrorFlip(uint32_t Instance, uint32_t MirrorFlip);
int32_t BSP_CAMERA_GetMirrorFlip(uint32_t Instance, uint32_t *MirrorFlip);

int32_t BSP_CAMERA_SetZoom(uint32_t Instance, uint32_t Zoom);
int32_t BSP_CAMERA_GetZoom(uint32_t Instance, uint32_t *Zoom);

int32_t BSP_CAMERA_EnableNightMode(uint32_t Instance);
int32_t BSP_CAMERA_DisableNightMode(uint32_t Instance);

int32_t BSP_CAMERA_HwReset(uint32_t Instance);
//int32_t BSP_CAMERA_PwrDown(uint32_t Instance);
//int32_t BSP_CAMERA_PwrUp(uint32_t Instance);
//void    BSP_CAMERA_LineEventCallback(uint32_t Instance);
//void    BSP_CAMERA_FrameEventCallback(uint32_t Instance);
//void    BSP_CAMERA_VsyncEventCallback(uint32_t Instance);
//void    BSP_CAMERA_ErrorCallback(uint32_t Instance);

void    BSP_CAMERA_IRQHandler(uint32_t Instance);
void    BSP_CAMERA_DMA_IRQHandler(uint32_t Instance);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_DCMI_Init(DCMI_HandleTypeDef* hdcmi);

/**
  * @}
  */


/** stm32h747i_discovery_errno.h **/
/* Common Error codes */
#define BSP_ERROR_NONE                    0
#define BSP_ERROR_NO_INIT                -1
#define BSP_ERROR_WRONG_PARAM            -2
#define BSP_ERROR_BUSY                   -3
#define BSP_ERROR_PERIPH_FAILURE         -4
#define BSP_ERROR_COMPONENT_FAILURE      -5
#define BSP_ERROR_UNKNOWN_FAILURE        -6
#define BSP_ERROR_UNKNOWN_COMPONENT      -7
#define BSP_ERROR_BUS_FAILURE            -8
#define BSP_ERROR_CLOCK_FAILURE          -9
#define BSP_ERROR_MSP_FAILURE            -10
#define BSP_ERROR_FEATURE_NOT_SUPPORTED  -11

/* BSP OSPI error codes */
#define BSP_ERROR_QSPI_ASSIGN_FAILURE     -24
#define BSP_ERROR_QSPI_SETUP_FAILURE      -25
#define BSP_ERROR_QSPI_MMP_LOCK_FAILURE   -26
#define BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE -27

/* BSP TS error code */
#define BSP_ERROR_TS_TOUCH_NOT_DETECTED   -30

/* BSP BUS error codes */
#define BSP_ERROR_BUS_TRANSACTION_FAILURE    -100
#define BSP_ERROR_BUS_ARBITRATION_LOSS       -101
#define BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE    -102
#define BSP_ERROR_BUS_PROTOCOL_FAILURE       -103

#define BSP_ERROR_BUS_MODE_FAULT             -104
#define BSP_ERROR_BUS_FRAME_ERROR            -105
#define BSP_ERROR_BUS_CRC_ERROR              -106
#define BSP_ERROR_BUS_DMA_FAILURE            -107

int32_t CAMERA_5640_Init(uint32_t Instance, uint32_t Resolution, uint32_t PixelFormat);
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi);

/* H7 camera.h */
typedef struct
{
  int32_t  (*Init              )(void*, uint32_t, uint32_t);
  int32_t  (*DeInit            )(void*);
  int32_t  (*ReadID            )(void*, uint32_t*);
  int32_t  (*GetCapabilities   )(void*, void*);
  int32_t  (*SetLightMode      )(void*, uint32_t);
  int32_t  (*SetColorEffect    )(void*, uint32_t);
  int32_t  (*SetBrightness     )(void*, int32_t);
  int32_t  (*SetSaturation     )(void*, int32_t);
  int32_t  (*SetContrast       )(void*, int32_t);
  int32_t  (*SetHueDegree      )(void*, int32_t);
  int32_t  (*MirrorFlipConfig  )(void*, uint32_t);
  int32_t  (*ZoomConfig        )(void*, uint32_t);
  int32_t  (*SetResolution     )(void*, uint32_t);
  int32_t  (*GetResolution     )(void*, uint32_t*);
  int32_t  (*SetPixelFormat    )(void*, uint32_t);
  int32_t  (*GetPixelFormat    )(void*, uint32_t*);
  int32_t  (*NightModeConfig   )(void*, uint32_t);
}CAMERA_Drv_t;


/* H7 discovery_conf.h */
/* COM define */
#define USE_COM_LOG                         0U
#define USE_BSP_COM_FEATURE                 0U
/* LCD controllers defines */
#define USE_LCD_CTRL_OTM8009A               1U
#define USE_LCD_CTRL_ADV7533                0U

#define LCD_LAYER_0_ADDRESS                 0xD0000000U
#define LCD_LAYER_1_ADDRESS                 0xD0200000U

#define USE_DMA2D_TO_FILL_RGB_RECT          0U
/* Camera sensors defines */
//#define USE_CAMERA_SENSOR_OV5640            1U
//#define USE_CAMERA_SENSOR_OV9655            1U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_WM8994              1U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        64U
/* TS supported features defines */
#define USE_TS_GESTURE                      1U
#define USE_TS_MULTI_TOUCH                  1U

/* Default TS touch number */
#define TS_TOUCH_NBR                        2U

/* IRQ priorities */
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_CAMERA_IT_PRIORITY              15U
#define BSP_BUTTON_WAKEUP_IT_PRIORITY       15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U
#define BSP_AUDIO_IN_IT_PRIORITY            15U
#define BSP_SD_IT_PRIORITY                  14U
#define BSP_SD_RX_IT_PRIORITY               14U
#define BSP_SD_TX_IT_PRIORITY               15U
#define BSP_TS_IT_PRIORITY                  15U
#define BSP_JOY1_SEL_IT_PRIORITY            15U
#define BSP_JOY1_DOWN_IT_PRIORITY           15U
#define BSP_JOY1_LEFT_IT_PRIORITY           15U
#define BSP_JOY1_RIGHT_IT_PRIORITY          15U


/* H747xx.h */

#define DMA2_Stream3_IRQn 59

//typedef enum
//{
///******  Cortex-M Processor Exceptions Numbers *****************************************************************/
//  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
//  HardFault_IRQn              = -13,    /*!< 3 Cortex-M Hard Fault Interrupt                                   */
//  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M Memory Management Interrupt                            */
//  BusFault_IRQn               = -11,    /*!< 5 Cortex-M Bus Fault Interrupt                                    */
//  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M Usage Fault Interrupt                                  */
//  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M SV Call Interrupt                                     */
//  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M Debug Monitor Interrupt                               */
//  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M Pend SV Interrupt                                     */
//  SysTick_IRQn                = -1,     /*!< 15 Cortex-M System Tick Interrupt                                 */
///******  STM32 specific Interrupt Numbers **********************************************************************/
//  WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt ( wwdg1_it, wwdg2_it)                   */
//  PVD_AVD_IRQn                = 1,      /*!< PVD/AVD through EXTI Line detection Interrupt                     */
//  TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
//  RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
//  FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
//  RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
//  EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
//  EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
//  EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
//  EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
//  EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
//  DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
//  DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
//  DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
//  DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
//  DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
//  DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
//  DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
//  ADC_IRQn                    = 18,     /*!< ADC1 and  ADC2 global Interrupts                                  */
//  FDCAN1_IT0_IRQn             = 19,     /*!< FDCAN1 Interrupt line 0                                           */
//  FDCAN2_IT0_IRQn             = 20,     /*!< FDCAN2 Interrupt line 0                                           */
//  FDCAN1_IT1_IRQn             = 21,     /*!< FDCAN1 Interrupt line 1                                           */
//  FDCAN2_IT1_IRQn             = 22,     /*!< FDCAN2 Interrupt line 1                                           */
//  EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
//  TIM1_BRK_IRQn               = 24,     /*!< TIM1 Break Interrupt                                              */
//  TIM1_UP_IRQn                = 25,     /*!< TIM1 Update Interrupt                                             */
//  TIM1_TRG_COM_IRQn           = 26,     /*!< TIM1 Trigger and Commutation Interrupt                            */
//  TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
//  TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
//  TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
//  TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
//  I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
//  I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
//  I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
//  I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
//  SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
//  SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
//  USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
//  USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
//  USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
//  EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
//  RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
//  TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
//  TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
//  TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
//  TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
//  DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
//  FMC_IRQn                    = 48,     /*!< FMC global Interrupt                                              */
//  SDMMC1_IRQn                 = 49,     /*!< SDMMC1 global Interrupt                                           */
//  TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
//  SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
//  UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
//  UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
//  TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
//  TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
//  DMA2_Stream0_IRQn           = 56,     /*!<   DMA2 Stream 0 global Interrupt                                  */
//  DMA2_Stream1_IRQn           = 57,     /*!<   DMA2 Stream 1 global Interrupt                                  */
//  DMA2_Stream2_IRQn           = 58,     /*!<   DMA2 Stream 2 global Interrupt                                  */
//  DMA2_Stream3_IRQn           = 59,     /*!<   DMA2 Stream 3 global Interrupt                                  */
//  DMA2_Stream4_IRQn           = 60,     /*!<   DMA2 Stream 4 global Interrupt                                  */
//  ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
//  ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
//  FDCAN_CAL_IRQn              = 63,     /*!< FDCAN Calibration unit Interrupt                                  */
//  CM7_SEV_IRQn                = 64,     /*!< CM7 Send event interrupt for CM4                                  */
//  CM4_SEV_IRQn                = 65,     /*!< CM4 Send event interrupt for CM7                                  */
//  DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
//  DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
//  DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
//  USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
//  I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
//  I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
//  OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
//  OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
//  OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
//  OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
//  DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
//  RNG_IRQn                    = 80,     /*!< RNG global interrupt                                              */
//  FPU_IRQn                    = 81,     /*!< FPU global interrupt                                              */
//  UART7_IRQn                  = 82,     /*!< UART7 global interrupt                                            */
//  UART8_IRQn                  = 83,     /*!< UART8 global interrupt                                            */
//  SPI4_IRQn                   = 84,     /*!< SPI4 global Interrupt                                             */
//  SPI5_IRQn                   = 85,     /*!< SPI5 global Interrupt                                             */
//  SPI6_IRQn                   = 86,     /*!< SPI6 global Interrupt                                             */
//  SAI1_IRQn                   = 87,     /*!< SAI1 global Interrupt                                             */
//  LTDC_IRQn                   = 88,     /*!< LTDC global Interrupt                                             */
//  LTDC_ER_IRQn                = 89,     /*!< LTDC Error global Interrupt                                       */
//  DMA2D_IRQn                  = 90,     /*!< DMA2D global Interrupt                                            */
//  SAI2_IRQn                   = 91,     /*!< SAI2 global Interrupt                                             */
//  QUADSPI_IRQn                = 92,     /*!< Quad SPI global interrupt                                         */
//  LPTIM1_IRQn                 = 93,     /*!< LP TIM1 interrupt                                                 */
//  CEC_IRQn                    = 94,     /*!< HDMI-CEC global Interrupt                                         */
//  I2C4_EV_IRQn                = 95,     /*!< I2C4 Event Interrupt                                              */
//  I2C4_ER_IRQn                = 96,     /*!< I2C4 Error Interrupt                                              */
//  SPDIF_RX_IRQn               = 97,     /*!< SPDIF-RX global Interrupt                                         */
//  OTG_FS_EP1_OUT_IRQn         = 98,     /*!< USB OTG HS2 global interrupt                                      */
//  OTG_FS_EP1_IN_IRQn          = 99,     /*!< USB OTG HS2 End Point 1 Out global interrupt                      */
//  OTG_FS_WKUP_IRQn            = 100,    /*!< USB OTG HS2 End Point 1 In global interrupt                       */
//  OTG_FS_IRQn                 = 101,    /*!< USB OTG HS2 Wakeup through EXTI interrupt                         */
//  DMAMUX1_OVR_IRQn            = 102,    /*!<DMAMUX1 Overrun interrupt                                          */
//  HRTIM1_Master_IRQn          = 103,    /*!< HRTIM Master Timer global Interrupts                              */
//  HRTIM1_TIMA_IRQn            = 104,    /*!< HRTIM Timer A global Interrupt                                    */
//  HRTIM1_TIMB_IRQn            = 105,    /*!< HRTIM Timer B global Interrupt                                    */
//  HRTIM1_TIMC_IRQn            = 106,    /*!< HRTIM Timer C global Interrupt                                    */
//  HRTIM1_TIMD_IRQn            = 107,    /*!< HRTIM Timer D global Interrupt                                    */
//  HRTIM1_TIME_IRQn            = 108,    /*!< HRTIM Timer E global Interrupt                                    */
//  HRTIM1_FLT_IRQn             = 109,    /*!< HRTIM Fault global Interrupt                                      */
//  DFSDM1_FLT0_IRQn            = 110,    /*!<DFSDM Filter1 Interrupt                                            */
//  DFSDM1_FLT1_IRQn            = 111,    /*!<DFSDM Filter2 Interrupt                                            */
//  DFSDM1_FLT2_IRQn            = 112,    /*!<DFSDM Filter3 Interrupt                                            */
//  DFSDM1_FLT3_IRQn            = 113,    /*!<DFSDM Filter4 Interrupt                                            */
//  SAI3_IRQn                   = 114,    /*!< SAI3 global Interrupt                                             */
//  SWPMI1_IRQn                 = 115,    /*!< Serial Wire Interface 1 global interrupt                          */
//  TIM15_IRQn                  = 116,    /*!< TIM15 global Interrupt                                            */
//  TIM16_IRQn                  = 117,    /*!< TIM16 global Interrupt                                            */
//  TIM17_IRQn                  = 118,    /*!< TIM17 global Interrupt                                            */
//  MDIOS_WKUP_IRQn             = 119,    /*!< MDIOS Wakeup  Interrupt                                           */
//  MDIOS_IRQn                  = 120,    /*!< MDIOS global Interrupt                                            */
//  JPEG_IRQn                   = 121,    /*!< JPEG global Interrupt                                             */
//  MDMA_IRQn                   = 122,    /*!< MDMA global Interrupt                                             */
//  DSI_IRQn                    = 123,    /*!< DSI global Interrupt                                              */
//  SDMMC2_IRQn                 = 124,    /*!< SDMMC2 global Interrupt                                           */
//  HSEM1_IRQn                  = 125,    /*!< HSEM1 global Interrupt                                            */
//  HSEM2_IRQn                  = 126,    /*!< HSEM2 global Interrupt                                            */
//  ADC3_IRQn                   = 127,    /*!< ADC3 global Interrupt                                             */
//  DMAMUX2_OVR_IRQn            = 128,    /*!<DMAMUX2 Overrun interrupt                                          */
//  BDMA_Channel0_IRQn          = 129,    /*!< BDMA Channel 0 global Interrupt                                   */
//  BDMA_Channel1_IRQn          = 130,    /*!< BDMA Channel 1 global Interrupt                                   */
//  BDMA_Channel2_IRQn          = 131,    /*!< BDMA Channel 2 global Interrupt                                   */
//  BDMA_Channel3_IRQn          = 132,    /*!< BDMA Channel 3 global Interrupt                                   */
//  BDMA_Channel4_IRQn          = 133,    /*!< BDMA Channel 4 global Interrupt                                   */
//  BDMA_Channel5_IRQn          = 134,    /*!< BDMA Channel 5 global Interrupt                                   */
//  BDMA_Channel6_IRQn          = 135,    /*!< BDMA Channel 6 global Interrupt                                   */
//  BDMA_Channel7_IRQn          = 136,    /*!< BDMA Channel 7 global Interrupt                                   */
//  COMP_IRQn                   = 137 ,   /*!< COMP global Interrupt                                             */
//  LPTIM2_IRQn                 = 138,    /*!< LP TIM2 global interrupt                                          */
//  LPTIM3_IRQn                 = 139,    /*!< LP TIM3 global interrupt                                          */
//  LPTIM4_IRQn                 = 140,    /*!< LP TIM4 global interrupt                                          */
//  LPTIM5_IRQn                 = 141,    /*!< LP TIM5 global interrupt                                          */
//  LPUART1_IRQn                = 142,    /*!< LP UART1 interrupt                                                */
//  WWDG_RST_IRQn               = 143,    /*!<Window Watchdog reset interrupt (exti_d2_wwdg_it, exti_d1_wwdg_it) */
//  CRS_IRQn                    = 144,    /*!< Clock Recovery Global Interrupt                                   */
//  ECC_IRQn                    = 145,    /*!< ECC diagnostic Global Interrupt                                   */
//  SAI4_IRQn                   = 146,    /*!< SAI4 global interrupt                                             */
//  HOLD_CORE_IRQn              = 148,    /*!< Hold core interrupt                                               */
//  WAKEUP_PIN_IRQn             = 149,    /*!< Interrupt for all 6 wake-up pins                                  */
//} IRQn_Type;
//
