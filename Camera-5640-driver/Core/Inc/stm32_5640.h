#include "stm32l496g_discovery.h"
#include "stm32l496g_discovery_camera.h"
#include "camera.h"
/** @defgroup STM32H747I_DISCO_CAMERA_Exported_Constants Exported Constants
  * @{
  */
/* Camera instance number */
/* H7 discovery_camera.h */
#ifndef USE_CAMERA_SENSOR_OV9655
#define USE_CAMERA_SENSOR_OV9655         0
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

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(I2C_HandleTypeDef *);
  void (* pMspDeInitCb)(I2C_HandleTypeDef *);
}BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */
