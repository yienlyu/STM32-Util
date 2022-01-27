################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.c \
/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.c \
/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.c 

OBJS += \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.o \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.o \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.o 

C_DEPS += \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.d \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.d \
./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.o: /Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.c Drivers/BSP/STM32L496G-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/Components/Common -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/drivers -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.o: /Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.c Drivers/BSP/STM32L496G-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/Components/Common -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/drivers -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.o: /Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.c Drivers/BSP/STM32L496G-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/Components/Common -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/drivers -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32L496G-2d-Discovery

clean-Drivers-2f-BSP-2f-STM32L496G-2d-Discovery:
	-$(RM) ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.d ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery.o ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.d ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_io.o ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.d ./Drivers/BSP/STM32L496G-Discovery/stm32l496g_discovery_sd.o

.PHONY: clean-Drivers-2f-BSP-2f-STM32L496G-2d-Discovery

