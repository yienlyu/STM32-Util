- "unknown type name" error gone when the `Inc` and `Src` are under `Core` directory
	- Somehow it affects the IDE to find the path or not, currently using separate directories (`Inc` and `Src`)
- The H7 camera driver has a `MX_DCMI_Init()` defined in `discovery_camera.c`. However, the L4 driver typically uses `MX_DCMI_Init()` at the beginning of main function; that is, it would be defined well in `main.c`. Although the one in H7 driver is labeled `__weak`, it still could be wrongly called. 
	- The `MX_DCMI_Init()` is used to initializes the DCMI peripheral, which are always initialized at the beginning of `CAMERA_5640_Init()`
- Checked the `CAMERA_5640_Init()` function compare to both H7 and L4 drivers and has nothing wrongly called right now (hopefully)
- Also checked again the MFX pin `DCMI_PWR_EN` in `MX_GPIO_Init()`, taking reference of the `SD_detect_Pin`.
![](https://i.imgur.com/JYimo6D.png)
- Checked all the PINs initialization, and used `BSP_CAMERA_FrameEventCallback()` to check the LCD write data through DMA2D, but still no correct picture captured. (So is there any problem with DMA2D? However the data transfer works well with `DMA2D_Config()`. On the other hand, it doesn't work if only runs `MX_DMA2D_Init()`, which is still a mystery to me.)

