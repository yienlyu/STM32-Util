################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FATFS/App/fatfs.c 

OBJS += \
./FATFS/App/fatfs.o 

C_DEPS += \
./FATFS/App/fatfs.d 


# Each subdirectory must supply rules for building sources it contributes
FATFS/App/%.o: ../FATFS/App/%.c FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/STM32L496G-Discovery -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/BSP/Components/Common -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Drivers/CMSIS/Include -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Middlewares/Third_Party/FatFs/src/drivers -I/Users/natalielyu/Downloads/STM32Cube_FW_L4_V1.17.0/Projects/32L496GDISCOVERY/Applications/FatFs/FatFs_uSD_Standalone/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FATFS-2f-App

clean-FATFS-2f-App:
	-$(RM) ./FATFS/App/fatfs.d ./FATFS/App/fatfs.o

.PHONY: clean-FATFS-2f-App

