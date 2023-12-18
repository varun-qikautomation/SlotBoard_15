################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MODBUS-LIB/Src/Modbus.c \
../MODBUS-LIB/Src/UARTCallback.c 

OBJS += \
./MODBUS-LIB/Src/Modbus.o \
./MODBUS-LIB/Src/UARTCallback.o 

C_DEPS += \
./MODBUS-LIB/Src/Modbus.d \
./MODBUS-LIB/Src/UARTCallback.d 


# Each subdirectory must supply rules for building sources it contributes
MODBUS-LIB/Src/%.o MODBUS-LIB/Src/%.su: ../MODBUS-LIB/Src/%.c MODBUS-LIB/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G031xx -c -I../Core/Inc -I"D:/Smart Projector/Firmware/STM32G0/SlotBoard_stm32g0_v1p0/MODBUS-LIB/Inc" -I"D:/Smart Projector/Firmware/STM32G0/SlotBoard_stm32g0_v1p0/MODBUS-LIB/Config" -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MODBUS-2d-LIB-2f-Src

clean-MODBUS-2d-LIB-2f-Src:
	-$(RM) ./MODBUS-LIB/Src/Modbus.d ./MODBUS-LIB/Src/Modbus.o ./MODBUS-LIB/Src/Modbus.su ./MODBUS-LIB/Src/UARTCallback.d ./MODBUS-LIB/Src/UARTCallback.o ./MODBUS-LIB/Src/UARTCallback.su

.PHONY: clean-MODBUS-2d-LIB-2f-Src

