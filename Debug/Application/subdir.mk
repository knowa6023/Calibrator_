################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/application.c \
../Application/mcu_communication.c \
../Application/timer.c 

OBJS += \
./Application/application.o \
./Application/mcu_communication.o \
./Application/timer.o 

C_DEPS += \
./Application/application.d \
./Application/mcu_communication.d \
./Application/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o: ../Application/%.c Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/FM/STM32CubeIDE/workspace_1.7.0/Calibrator_/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

