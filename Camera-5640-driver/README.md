- "unknown type name" error gone when the `Inc` and `Src` are under `Core` directory
	- Somehow it affects the IDE to find the path or not, currently using separate directorys (`Inc` and `Src`)
- The H7 camera driver has a `MX_DCMI_Init()` defined in `discovery_camera.c`. However, the L4 driver typically uses `MX_DCMI_Init()` at the beginning of main function; that is, it would be defined well in `main.c`. Although the one in H7 driver is labeled `__weak`, it still could be wrongly called. 
- Checked the `CAMERA_5640_Init()` function compare to both H7 and L4 drivers (hopefully!)
- Haven't check `BSP_CANERA_Start()` yet
- Also checked again the MFX pin `DCMI_PWR_EN`.
![](https://i.imgur.com/rayQ6vr.png)

