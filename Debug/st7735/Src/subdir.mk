################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../st7735/Src/fonts.c \
../st7735/Src/st7735.c 

OBJS += \
./st7735/Src/fonts.o \
./st7735/Src/st7735.o 

C_DEPS += \
./st7735/Src/fonts.d \
./st7735/Src/st7735.d 


# Each subdirectory must supply rules for building sources it contributes
st7735/Src/%.o: ../st7735/Src/%.c st7735/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32/STM32CubeIDE/DDS_AD9833/st7735/Inc" -I"D:/STM32/STM32CubeIDE/DDS_AD9833/AD9833/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

