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
