#include "../Inc/stm32_5640.h"

#include "stm32l496g_discovery_camera.h"
#include "ov9655.h"
#include "ov5640.h"
#include "stm32l496g_discovery.h"
#include "stm32l496g_discovery_io.h"

#include "../Inc/stm32_5640_bus.h"

#define DCMI_PWR_EN_Pin GPIO_PIN_6
#define DCMI_PWR_EN_GPIO_Port GPIOC

// STM32H747I_DISCO_CAMERA_Private_Variables Private Variables
static CAMERA_Drv_t *Camera_Drv = NULL;
static CAMERA_Capabilities_t Camera_Cap;
static uint32_t HSPolarity = DCMI_HSPOLARITY_LOW;
static uint32_t CameraId;


// STM32H747I_DISCO_CAMERA_Private_Variables Private Variables
void                *Camera_CompObj = NULL;
DCMI_HandleTypeDef  hcamera_dcmi;
CAMERA_Ctx_t        Camera_Ctx[CAMERA_INSTANCES_NBR];

// STM32H747I_DISCO_CAMERA_Private_FunctionsPrototypes Private Functions Prototypes
static int32_t GetSize(uint32_t Resolution, uint32_t PixelFormat);
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi);
static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi);
#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
static void DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi);
static void DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);
static void DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi);
static void DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi);
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 1) */

#if (USE_CAMERA_SENSOR_OV9655 == 1)
static int32_t OV9655_Probe(uint32_t Resolution, uint32_t PixelFormat);
#endif

#if (USE_CAMERA_SENSOR_OV5640 == 1)
static int32_t OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat);
#endif
/**
  * @}
  */
#define CAMERA_VGA_RES_X          640
#define CAMERA_VGA_RES_Y          480
#define CAMERA_480x272_RES_X      480
#define CAMERA_480x272_RES_Y      272
#define CAMERA_QVGA_RES_X         320
#define CAMERA_QVGA_RES_Y         240
#define CAMERA_QQVGA_RES_X        160
#define CAMERA_QQVGA_RES_Y        120



//DCMI_HandleTypeDef  hDcmiHandler;
CAMERA_DrvTypeDef   *camera_drv;
/* Camera current resolution naming (QQVGA, VGA, ...) */
static uint32_t CameraCurrentResolution;

/* Camera module I2C HW address */
static uint32_t CameraHwAddress;

// replace BSP_CAMERA_Init
uint8_t CAMERA_5640_Init(uint32_t Instance, uint32_t Resolution, uint32_t PixelFormat) {
	DCMI_HandleTypeDef *phdcmi;
	uint8_t status = CAMERA_ERROR;
	int32_t ret = BSP_ERROR_NONE;

	/* Get the DCMI handle structure */
	phdcmi = &hcamera_dcmi;


	/* Initialize the IO functionalities */
	BSP_IO_Init();
//	BSP_IO_ConfigPin(CAMERA_PWR_EN_PIN, IO_MODE_OUTPUT);

	/* Camera initialization */
	// Initializes the DCMI MSP -> ok
	// DCMI_MspInit in H7
	BSP_CAMERA_MspInit(&hcamera_dcmi, NULL);

	/* Initialize the camera driver structure */
	// H7
//    if(MX_DCMI_Init(&hcamera_dcmi) != HAL_OK)
//    {
//      ret = BSP_ERROR_PERIPH_FAILURE;
//    }
	/*** Configures the DCMI to interface with the camera module ***/
	/* DCMI configuration */
	phdcmi->Init.CaptureRate      = DCMI_CR_ALL_FRAME;
	phdcmi->Init.HSPolarity       = DCMI_HSPOLARITY_HIGH;
	phdcmi->Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
	phdcmi->Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
	phdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
	phdcmi->Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
	phdcmi->Init.ByteSelectMode   = DCMI_BSM_ALL;
	phdcmi->Init.LineSelectMode   = DCMI_LSM_ALL;
	phdcmi->Instance              = DCMI;


    if(BSP_CAMERA_HwReset(0) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
#if (USE_CAMERA_SENSOR_OV9655 == 1)
      ret= OV9655_Probe(Resolution, PixelFormat);
#endif
#if (USE_CAMERA_SENSOR_OV5640 == 1)
      if(ret != BSP_ERROR_NONE)
      {
        ret = OV5640_Probe(Resolution, PixelFormat);
      }
#endif
      if(ret != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_UNKNOWN_COMPONENT;
      }
      else
      {
        if((CameraId == OV9655_ID) || (CameraId == OV9655_ID_2))
        {
          if(Resolution == CAMERA_R480x272)
          {
            if(HAL_DCMI_ConfigCROP(&hcamera_dcmi,           /* Crop in the middle of the VGA picture */
                                   (CAMERA_VGA_RES_X - CAMERA_480x272_RES_X)/2,
                                   (CAMERA_VGA_RES_Y - CAMERA_480x272_RES_Y)/2,
                                   (CAMERA_480x272_RES_X * 2) - 1,
                                   CAMERA_480x272_RES_Y - 1) != HAL_OK)
            {
              ret = BSP_ERROR_PERIPH_FAILURE;
            }
            else
            {
              if(HAL_DCMI_EnableCROP(&hcamera_dcmi) != HAL_OK)
              {
                ret = BSP_ERROR_PERIPH_FAILURE;
              }
            }
          }
        }
        else
        {
          HSPolarity = DCMI_HSPOLARITY_HIGH;
          // already initialized
          /* Initialize the camera driver structure */
//          if(MX_DCMI_Init(&hcamera_dcmi) != HAL_OK)
//          {
//            ret = BSP_ERROR_PERIPH_FAILURE;
//          }
        }

        if(ret == BSP_ERROR_NONE)
        {
          if(BSP_CAMERA_HwReset(0) != BSP_ERROR_NONE)
          {
            ret = BSP_ERROR_BUS_FAILURE;
          }
        }

        if(ret == BSP_ERROR_NONE)
        {
          Camera_Ctx[Instance].CameraId  = CameraId;
#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
          /* Register DCMI LineEvent, FrameEvent and Error callbacks */
          if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_LINE_EVENT_CB_ID, DCMI_LineEventCallback) != HAL_OK)
          {
            ret = BSP_ERROR_PERIPH_FAILURE;
          }
          else if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_FRAME_EVENT_CB_ID, DCMI_FrameEventCallback) != HAL_OK)
          {
            ret = BSP_ERROR_PERIPH_FAILURE;
          }
          else if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_VSYNC_EVENT_CB_ID, DCMI_VsyncEventCallback) != HAL_OK)
          {
            ret = BSP_ERROR_PERIPH_FAILURE;
          }
          else if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_ERROR_CB_ID, DCMI_ErrorCallback) != HAL_OK)
          {
            ret = BSP_ERROR_PERIPH_FAILURE;
          }
          else
          {
            ret = BSP_ERROR_NONE;
          }
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 1) */
        }
        if(ret == BSP_ERROR_NONE)
        {
          Camera_Ctx[Instance].Resolution  = Resolution;
          Camera_Ctx[Instance].PixelFormat = PixelFormat;
        }
      }
    }

  /* BSP status */
  return ret;

}
//
//int32_t BSP_CAMERA_DeInit(uint32_t Instance)
//{
//  int32_t ret;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else
//  {
//    if((Camera_CompObj == NULL)||(Camera_Drv == NULL))
//    {
//      ret = BSP_ERROR_NO_INIT;
//    }
//    else
//    {
//      hcamera_dcmi.Instance = DCMI;
//
//      /* First stop the camera to insure all data are transferred */
//      if(BSP_CAMERA_Stop(Instance) != BSP_ERROR_NONE)
//      {
//        ret = BSP_ERROR_PERIPH_FAILURE;
//      }
//      else if(HAL_DCMI_DisableCROP(&hcamera_dcmi)!= HAL_OK)
//      {
//        ret = BSP_ERROR_PERIPH_FAILURE;
//      }
//      else if(HAL_DCMI_DeInit(&hcamera_dcmi) != HAL_OK)
//      {
//        ret = BSP_ERROR_PERIPH_FAILURE;
//      }
//      else
//      {
//#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 0)
//        DCMI_MspDeInit(&hcamera_dcmi);
//#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 0) */
//
//        if(Camera_Drv->DeInit(Camera_CompObj) != BSP_ERROR_NONE)
//        {
//          ret = BSP_ERROR_COMPONENT_FAILURE;
//        }/* Set Camera in Power Down */
//        else if(BSP_CAMERA_PwrDown(0) != BSP_ERROR_NONE)
//        {
//          ret = BSP_ERROR_BUS_FAILURE;
//        }
//        else
//        {
//          ret = BSP_ERROR_NONE;
//        }
//      }
//    }
//  }
//  /* Return BSP status */
//  return ret;
//}


#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP CAMERA Msp Callbacks
  * @param Instance CAMERA Instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_DCMI_RESET_HANDLE_STATE(&hcamera_dcmi);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_MSPINIT_CB_ID, DCMI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_MSPDEINIT_CB_ID, DCMI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Camera_Ctx[Instance].IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP CAMERA Msp Callback registering
  * @param Instance     CAMERA Instance.
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_CAMERA_RegisterMspCallbacks(uint32_t Instance, BSP_CAMERA_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_DCMI_RESET_HANDLE_STATE(&hcamera_dcmi);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_DCMI_RegisterCallback(&hcamera_dcmi, HAL_DCMI_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      Camera_Ctx[Instance].IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Starts the camera capture in continuous mode.
  * @param  Instance Camera instance.
  * @param  pBff     pointer to the camera output buffer
  * @param  Mode CAMERA_MODE_CONTINUOUS or CAMERA_MODE_SNAPSHOT
  * @retval BSP status
  */
int32_t BSP_CAMERA_Start(uint32_t Instance, uint32_t pBff, uint32_t Mode)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_DCMI_Start_DMA(&hcamera_dcmi, Mode, (uint32_t)pBff, (uint32_t)GetSize(Camera_Ctx[Instance].Resolution, Camera_Ctx[Instance].PixelFormat)) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}



/**
  * @brief  Stop the CAMERA capture
  * @param  Instance Camera instance.
  * @retval BSP status
  */
//int32_t BSP_CAMERA_Stop(uint32_t Instance)
//{
//  int32_t ret;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else if(HAL_DCMI_Stop(&hcamera_dcmi) != HAL_OK)
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//  else
//  {
//    ret = BSP_ERROR_NONE;
//  }
//
//  /* Return BSP status */
//  return ret;
//}
//
///**
//  * @brief Suspend the CAMERA capture
//  * @param  Instance Camera instance.
//  */
//int32_t BSP_CAMERA_Suspend(uint32_t Instance)
//{
//  int32_t ret;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else if(HAL_DCMI_Suspend(&hcamera_dcmi) != HAL_OK)
//  {
//    return BSP_ERROR_PERIPH_FAILURE;
//  }
//  else
//  {
//    ret = BSP_ERROR_NONE;
//  }
//
//  /* Return BSP status */
//  return ret;
//}
//
///**
//  * @brief Resume the CAMERA capture
//  * @param  Instance Camera instance.
//  */
//int32_t BSP_CAMERA_Resume(uint32_t Instance)
//{
//  int32_t ret;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else if(HAL_DCMI_Resume(&hcamera_dcmi) != HAL_OK)
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//  else
//  {
//    ret = BSP_ERROR_NONE;
//  }
//
//  /* Return BSP status */
//  return ret;
//}

/**
  * @brief  Get the Camera Capabilities.
  * @param  Instance  Camera instance.
  * @param  Capabilities  pointer to camera Capabilities
  * @note   This function should be called after the init. This to get Capabilities
  *         from the right camera sensor(OV9655)
  * @retval Component status
  */
int32_t BSP_CAMERA_GetCapabilities(uint32_t Instance, CAMERA_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Drv->GetCapabilities(Camera_CompObj, Capabilities) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera pixel format.
  * @param  Instance  Camera instance.
  * @param  PixelFormat pixel format to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetPixelFormat(uint32_t Instance, uint32_t PixelFormat)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Drv->SetPixelFormat(Camera_CompObj, PixelFormat) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].PixelFormat = PixelFormat;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera pixel format.
  * @param  Instance  Camera instance.
  * @param  PixelFormat pixel format to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *PixelFormat = Camera_Ctx[Instance].PixelFormat;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}


/**
  * @brief  Set the camera Resolution.
  * @param  Instance  Camera instance.
  * @param  Resolution Resolution to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetResolution(uint32_t Instance, uint32_t Resolution)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Resolution == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetResolution(Camera_CompObj, Resolution) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].Resolution = Resolution;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Resolution.
  * @param  Instance  Camera instance.
  * @param  Resolution Resolution to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetResolution(uint32_t Instance, uint32_t *Resolution)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Resolution == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *Resolution = Camera_Ctx[Instance].Resolution;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Light Mode.
  * @param  Instance  Camera instance.
  * @param  LightMode Light Mode to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetLightMode(uint32_t Instance, uint32_t LightMode)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.LightMode == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetLightMode(Camera_CompObj, LightMode) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].LightMode = LightMode;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Light Mode.
  * @param  Instance  Camera instance.
  * @param  LightMode Light Mode to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetLightMode(uint32_t Instance, uint32_t *LightMode)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.LightMode == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *LightMode = Camera_Ctx[Instance].LightMode;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Special Effect.
  * @param  Instance Camera instance.
  * @param  ColorEffect Effect to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetColorEffect(uint32_t Instance, uint32_t ColorEffect)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.ColorEffect == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetColorEffect(Camera_CompObj, ColorEffect) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].ColorEffect = ColorEffect;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Special Effect.
  * @param  Instance Camera instance.
  * @param  ColorEffect Effect to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetColorEffect(uint32_t Instance, uint32_t *ColorEffect)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.ColorEffect == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *ColorEffect = Camera_Ctx[Instance].ColorEffect;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Brightness Level.
  * @param  Instance   Camera instance.
  * @param  Brightness Brightness Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetBrightness(uint32_t Instance, int32_t Brightness)
{
  int32_t ret;

  if((Instance >= CAMERA_INSTANCES_NBR) || ((Brightness < CAMERA_BRIGHTNESS_MIN) && (Brightness > CAMERA_BRIGHTNESS_MAX)))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Brightness == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetBrightness(Camera_CompObj, Brightness) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].Brightness = Brightness;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Brightness Level.
  * @param  Instance Camera instance.
  * @param  Brightness  Brightness Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetBrightness(uint32_t Instance, int32_t *Brightness)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Brightness == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *Brightness = Camera_Ctx[Instance].Brightness;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Saturation Level.
  * @param  Instance    Camera instance.
  * @param  Saturation  Saturation Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetSaturation(uint32_t Instance, int32_t Saturation)
{
  int32_t ret;

  if((Instance >= CAMERA_INSTANCES_NBR) || ((Saturation < CAMERA_SATURATION_MIN) && (Saturation > CAMERA_SATURATION_MAX)))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Saturation == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetSaturation(Camera_CompObj, Saturation)  < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].Saturation = Saturation;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Saturation Level.
  * @param  Instance    Camera instance.
  * @param  Saturation  Saturation Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetSaturation(uint32_t Instance, int32_t *Saturation)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Saturation == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *Saturation = Camera_Ctx[Instance].Saturation;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Contrast Level.
  * @param  Instance Camera instance.
  * @param  Contrast Contrast Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetContrast(uint32_t Instance, int32_t Contrast)
{
  int32_t ret;

  if((Instance >= CAMERA_INSTANCES_NBR) || ((Contrast < CAMERA_CONTRAST_MIN) && (Contrast > CAMERA_CONTRAST_MAX)))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Contrast == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetContrast(Camera_CompObj, Contrast)  < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].Contrast = Contrast;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Contrast Level.
  * @param  Instance Camera instance.
  * @param  Contrast Contrast Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetContrast(uint32_t Instance, int32_t *Contrast)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Contrast == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *Contrast = Camera_Ctx[Instance].Contrast;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Hue Degree.
  * @param  Instance   Camera instance.
  * @param  HueDegree  Hue Degree
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetHueDegree(uint32_t Instance, int32_t HueDegree)
{
  int32_t ret;

  if((Instance >= CAMERA_INSTANCES_NBR) || ((HueDegree < CAMERA_HUEDEGREE_MIN) && (HueDegree > CAMERA_HUEDEGREE_MAX)))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.HueDegree == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->SetHueDegree(Camera_CompObj, HueDegree) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].HueDegree = HueDegree;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Hue Degree.
  * @param  Instance   Camera instance.
  * @param  HueDegree  Hue Degree
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetHueDegree(uint32_t Instance, int32_t *HueDegree)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.HueDegree == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *HueDegree = Camera_Ctx[Instance].HueDegree;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera Mirror/Flip.
  * @param  Instance  Camera instance.
  * @param  MirrorFlip CAMERA_MIRRORFLIP_NONE or any combination of
  *                    CAMERA_MIRRORFLIP_FLIP and CAMERA_MIRRORFLIP_MIRROR
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetMirrorFlip(uint32_t Instance, uint32_t MirrorFlip)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.MirrorFlip == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->MirrorFlipConfig(Camera_CompObj, MirrorFlip)  < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].MirrorFlip = MirrorFlip;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera Mirror/Flip.
  * @param  Instance   Camera instance.
  * @param  MirrorFlip Mirror/Flip config
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetMirrorFlip(uint32_t Instance, uint32_t *MirrorFlip)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.MirrorFlip == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *MirrorFlip = Camera_Ctx[Instance].MirrorFlip;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set the camera zoom
  * @param  Instance Camera instance.
  * @param  Zoom     Zoom to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetZoom(uint32_t Instance, uint32_t Zoom)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Zoom == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->ZoomConfig(Camera_CompObj, Zoom) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Camera_Ctx[Instance].Zoom = Zoom;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Get the camera zoom
  * @param  Instance Camera instance.
  * @param  Zoom     Zoom to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetZoom(uint32_t Instance, uint32_t *Zoom)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.Zoom == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    *Zoom = Camera_Ctx[Instance].Zoom;
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Enable the camera night mode
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_EnableNightMode(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.NightMode == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->NightModeConfig(Camera_CompObj, CAMERA_NIGHT_MODE_SET) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Disable the camera night mode
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_DisableNightMode(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(Camera_Cap.NightMode == 0U)
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  else if(Camera_Drv->NightModeConfig(Camera_CompObj, CAMERA_NIGHT_MODE_RESET) < 0)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  CAMERA hardware reset
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_HwReset(uint32_t Instance)
{

  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpio_init_structure;

  if(Instance >= CAMERA_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Init DCMI PWR_ENABLE Pin */
    /* Enable GPIO clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    gpio_init_structure.Pin = DCMI_PWR_EN_Pin;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    /* De-assert the camera POWER_DOWN pin (active high) */
    HAL_GPIO_WritePin(GPIOC,DCMI_PWR_EN_Pin, GPIO_PIN_SET);

    HAL_Delay(100);     /* POWER_DOWN de-asserted during 100 ms */

    /* Assert the camera POWER_DOWN pin (active high) */
    HAL_GPIO_WritePin(GPIOC,DCMI_PWR_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
  }
  return ret;
}

/**
  * @brief  CAMERA power down
  * @param  Instance Camera instance.
  * @retval BSP status
  */
//int32_t BSP_CAMERA_PwrDown(uint32_t Instance)
//{
//  int32_t ret = BSP_ERROR_NONE;
//  GPIO_InitTypeDef gpio_init_structure;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else
//  {
//    /* Init DCMI PWR_ENABLE Pin */
//    /* Enable GPIO clock */
//    __HAL_RCC_GPIOH_CLK_ENABLE();
//
//    gpio_init_structure.Pin       = GPIO_PIN_14;
//    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
//    gpio_init_structure.Pull      = GPIO_NOPULL;
//    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOH, &gpio_init_structure);
//
//    /* Assert the camera POWER_DOWN pin (active high) */
//    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_SET);
//  }
//
//  return ret;
//}

/**
  * @brief  CAMERA power up
  * @param  Instance Camera instance.
  * @retval BSP status
  */
//int32_t BSP_CAMERA_PwrUp(uint32_t Instance)
//{
//  int32_t ret = BSP_ERROR_NONE;
//  GPIO_InitTypeDef gpio_init_structure;
//
//  if(Instance >= CAMERA_INSTANCES_NBR)
//  {
//    ret = BSP_ERROR_WRONG_PARAM;
//  }
//  else
//  {
//    /* Init DCMI PWR_ENABLE Pin */
//    /* Enable GPIO clock */
//    __HAL_RCC_GPIOH_CLK_ENABLE();
//
//    gpio_init_structure.Pin       = GPIO_PIN_14;
//    gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
//    gpio_init_structure.Pull      = GPIO_NOPULL;
//    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOH, &gpio_init_structure);
//
//    /* Assert the camera POWER_DOWN pin (active high) */
//    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);
//  }
//
//  return ret;
//}
/**
  * @brief  This function handles DCMI interrupt request.
  * @param  Instance Camera instance
  * @retval None
  */
void BSP_CAMERA_IRQHandler(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  HAL_DCMI_IRQHandler(&hcamera_dcmi);
}

/**
  * @brief  This function handles DCMI DMA interrupt request.
  * @param  Instance Camera instance
  * @retval None
  */
void BSP_CAMERA_DMA_IRQHandler(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  HAL_DMA_IRQHandler(hcamera_dcmi.DMA_Handle);
}

/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_CAMERA_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Get the capture size in pixels unit.
  * @param  Resolution  the current resolution.
  * @param  PixelFormat Camera pixel format
  * @retval capture size in 32-bit words.
  */
static int32_t GetSize(uint32_t Resolution, uint32_t PixelFormat)
{
  uint32_t size = 0;
  uint32_t pf_div;
  if(PixelFormat == CAMERA_PF_RGB888)
  {
    pf_div = 3; /* each pixel on 3 bytes so 3/4 words */
  }
  else
  {
    pf_div = 2; /* each pixel on 2 bytes so 1/2 words*/
  }
  /* Get capture size */
  switch (Resolution)
  {
  case CAMERA_R160x120:
    size =  ((uint32_t)(160*120)*pf_div)/4U;
    break;
  case CAMERA_R320x240:
    size =  ((uint32_t)(320*240)*pf_div)/4U;
    break;
  case CAMERA_R480x272:
    size =  ((uint32_t)(480*272)*pf_div)/4U;
    break;
  case CAMERA_R640x480:
    size =  ((uint32_t)(640*480)*pf_div)/4U;
//	  size =  ((uint32_t)(240*240)*pf_div)/4U;
    break;
  case CAMERA_R800x480:
    size =  ((uint32_t)(800*480)*pf_div)/4U;
    break;
  default:
    break;
  }

  return (int32_t)size;
}

/**
  * @brief  Initializes the DCMI MSP.
  * @param  hdcmi  DCMI handle
  * @retval None
  */
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi)
{
  static DMA_HandleTypeDef hdma_handler;
  GPIO_InitTypeDef gpio_init_structure;

  /*** Enable peripherals and GPIO clocks ***/
  /* Enable DCMI clock */
  __HAL_RCC_DCMI_CLK_ENABLE();

  /* Enable DMA2 clock */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* Enable GPIO clocks */

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_4);
  __HAL_RCC_HSI48_ENABLE();
  HAL_Delay(10); // HSI48 should start in 10ms

  /*** Configure the GPIO ***/
  /* Configure DCMI GPIO as alternate function */

  gpio_init_structure.Pin       = GPIO_PIN_14;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF10_DCMI;
  HAL_GPIO_Init(GPIOB, &gpio_init_structure);

  gpio_init_structure.Pin       = GPIO_PIN_5;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF10_DCMI;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 \
		  	  	  	  	  	  	| GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF10_DCMI;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF10_DCMI;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  /*** Configure the DMA ***/
  /* Set the parameters to be configured */
  hdma_handler.Instance                 = BSP_CAMERA_DMA_INSTANCE;
  hdma_handler.Init.Request             = DMA_REQUEST_0;

  hdma_handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_handler.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_handler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_handler.Init.Mode                = DMA_CIRCULAR;
  hdma_handler.Init.Priority            = DMA_PRIORITY_HIGH;

  // error?
//  hdma_handler.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
//  hdma_handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
//  hdma_handler.Init.MemBurst            = DMA_MBURST_SINGLE;
//  hdma_handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;
//  hdma_handler.Instance                 = DMA2_Stream3_IRQn;

  /* Associate the initialized DMA handle to the DCMI handle */
  __HAL_LINKDMA(hdcmi, DMA_Handle, hdma_handler);

  /*** Configure the NVIC for DCMI and DMA ***/
  /* NVIC configuration for DCMI transfer complete interrupt */
  HAL_NVIC_SetPriority(DCMI_IRQn, BSP_CAMERA_IT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(DCMI_IRQn);

  /* NVIC configuration for DMA2D transfer complete interrupt */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, BSP_CAMERA_IT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

  /* Configure the DMA stream */
  HAL_DMA_Init(hdcmi->DMA_Handle);
}

/**
  * @brief  DeInitializes the DCMI MSP.
  * @param  hdcmi  DCMI handle
  * @retval None
  */
static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Disable NVIC  for DCMI transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DCMI_IRQn);

  /* Disable NVIC for DMA2 transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DMA2_Stream3_IRQn);

  /* Configure the DMA stream */
  (void)HAL_DMA_DeInit(hdcmi->DMA_Handle);

  /* DeInit DCMI GPIOs */
  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_6;
  HAL_GPIO_DeInit(GPIOA, gpio_init_structure.Pin);

  gpio_init_structure.Pin       = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_DeInit(GPIOB, gpio_init_structure.Pin);

  gpio_init_structure.Pin       = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_11;
  HAL_GPIO_DeInit(GPIOC, gpio_init_structure.Pin);

  gpio_init_structure.Pin       = GPIO_PIN_3;
  HAL_GPIO_DeInit(GPIOD, gpio_init_structure.Pin);

  gpio_init_structure.Pin       = GPIO_PIN_10;
  HAL_GPIO_DeInit(GPIOG, gpio_init_structure.Pin);



  /* Disable DCMI clock */
  __HAL_RCC_DCMI_CLK_DISABLE();
}

#if (USE_HAL_DCMI_REGISTER_CALLBACKS == 0)
/**
  * @brief  Line event callback
  * @param  hdcmi  pointer to the DCMI handle
  * @retval None
  */
//void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hdcmi);
//
//  BSP_CAMERA_LineEventCallback();
//}
//
///**
//  * @brief  Frame event callback
//  * @param  hdcmi pointer to the DCMI handle
//  * @retval None
//  */
//void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hdcmi);
//
//  BSP_CAMERA_FrameEventCallback();
//}

/**
  * @brief  Vsync event callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
//void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hdcmi);
//
//  BSP_CAMERA_VsyncEventCallback(0);
//}

/**
  * @brief  Error callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
//void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(hdcmi);
//
//  BSP_CAMERA_ErrorCallback(0);
//}
#else
/**
  * @brief  Line event callback
  * @param  hdcmi  pointer to the DCMI handle
  * @retval None
  */
static void DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);

  BSP_CAMERA_LineEventCallback(0);
}

/**
  * @brief  Frame event callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
static void DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);

  BSP_CAMERA_FrameEventCallback(0);
}

/**
  * @brief  Vsync event callback
  * @param  hdcmi  pointer to the DCMI handle
  * @retval None
  */
static void DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);

  BSP_CAMERA_VsyncEventCallback(0);
}

/**
  * @brief  Error callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
static void DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);

  BSP_CAMERA_ErrorCallback(0);
}
#endif /* (USE_HAL_DCMI_REGISTER_CALLBACKS == 0) */

/**
  * @brief  Line Event callback.
  * @param  Instance Camera instance.
  * @retval None
  */
//__weak void BSP_CAMERA_LineEventCallback(uint32_t Instance)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(Instance);
//
//  /* NOTE : This function Should not be modified, when the callback is needed,
//            the HAL_DCMI_LineEventCallback could be implemented in the user file
//   */
//}
//
///**
//  * @brief  Frame Event callback.
//  * @param  Instance Camera instance.
//  * @retval None
//  */
//__weak void BSP_CAMERA_FrameEventCallback(uint32_t Instance)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(Instance);
//
//  /* NOTE : This function Should not be modified, when the callback is needed,
//            the HAL_DCMI_FrameEventCallback could be implemented in the user file
//   */
//}
//
///**
//  * @brief  Vsync Event callback.
//  * @param  Instance Camera instance.
//  * @retval None
//  */
//__weak void BSP_CAMERA_VsyncEventCallback(uint32_t Instance)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(Instance);
//
//  /* NOTE : This function Should not be modified, when the callback is needed,
//            the HAL_DCMI_FrameEventCallback could be implemented in the user file
//   */
//}
//
///**
//  * @brief  Error callback.
//  * @param  Instance Camera instance.
//  * @retval None
//  */
//__weak void BSP_CAMERA_ErrorCallback(uint32_t Instance)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(Instance);
//
//  /* NOTE : This function Should not be modified, when the callback is needed,
//            the HAL_DCMI_ErrorCallback could be implemented in the user file
//   */
//}

/**
  * @}
  */

/** @defgroup STM32H747I_DISCO_CAMERA_Private_Functions Private Functions
  * @{
  */

#if (USE_CAMERA_SENSOR_OV9655 == 1)
/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
static int32_t OV9655_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
  int32_t ret;
  OV9655_IO_t              IOCtx;
  static OV9655_Object_t   OV9655Obj;

  /* Configure the audio driver */
  IOCtx.Address     = CAMERA_OV9655_ADDRESS;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  if(OV9655_RegisterBusIO (&OV9655Obj, &IOCtx) != OV9655_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(OV9655_ReadID(&OV9655Obj, &CameraId) != OV9655_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    if((CameraId != OV9655_ID) && (CameraId != OV9655_ID_2))
    {
      ret = BSP_ERROR_UNKNOWN_COMPONENT;
    }
    else
    {
      Camera_Drv = (CAMERA_Drv_t *) &OV9655_CAMERA_Driver;
      Camera_CompObj = &OV9655Obj;
      if(Camera_Drv->Init(Camera_CompObj, Resolution, PixelFormat) != OV9655_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if(Camera_Drv->GetCapabilities(Camera_CompObj, &Camera_Cap) != OV9655_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }

  return ret;
}
#endif

#if (USE_CAMERA_SENSOR_OV5640 == 1)
/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
static int32_t OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
   int32_t ret;
  OV5640_IO_t              IOCtx;
  static OV5640_Object_t   OV5640Obj;

  /* Configure the audio driver */
  IOCtx.Address     = CAMERA_OV5640_ADDRESS;
  IOCtx.Init        = BSP_I2C2_Init;
  IOCtx.DeInit      = BSP_I2C2_DeInit;
  IOCtx.ReadReg     = BSP_I2C2_ReadReg16;
  IOCtx.WriteReg    = BSP_I2C2_WriteReg16;
  IOCtx.GetTick     = BSP_GetTick;

  if(OV5640_RegisterBusIO (&OV5640Obj, &IOCtx) != OV5640_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else if(OV5640_ReadID(&OV5640Obj, &CameraId) != OV5640_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    if(CameraId != OV5640_ID)
    {
      ret = BSP_ERROR_UNKNOWN_COMPONENT;
    }
    else
    {
      Camera_Drv = (CAMERA_Drv_t *) &OV5640_CAMERA_Driver;
      Camera_CompObj = &OV5640Obj;
      if(Camera_Drv->Init(Camera_CompObj, Resolution, PixelFormat) != OV5640_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if(Camera_Drv->GetCapabilities(Camera_CompObj, &Camera_Cap) != OV5640_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {

        ret = BSP_ERROR_NONE;
      }
    }
  }

  return ret;
}
#endif


