################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/IRremote.c \
../Core/Src/RGBLed.c \
../Core/Src/Slot.c \
../Core/Src/irSend.c \
../Core/Src/main.c \
../Core/Src/modbusSlave.c \
../Core/Src/modbus_crc.c \
../Core/Src/stm32g0xx_hal_msp.c \
../Core/Src/stm32g0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g0xx.c 

OBJS += \
./Core/Src/IRremote.o \
./Core/Src/RGBLed.o \
./Core/Src/Slot.o \
./Core/Src/irSend.o \
./Core/Src/main.o \
./Core/Src/modbusSlave.o \
./Core/Src/modbus_crc.o \
./Core/Src/stm32g0xx_hal_msp.o \
./Core/Src/stm32g0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g0xx.o 

C_DEPS += \
./Core/Src/IRremote.d \
./Core/Src/RGBLed.d \
./Core/Src/Slot.d \
./Core/Src/irSend.d \
./Core/Src/main.d \
./Core/Src/modbusSlave.d \
./Core/Src/modbus_crc.d \
./Core/Src/stm32g0xx_hal_msp.d \
./Core/Src/stm32g0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G031xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/IRremote.cyclo ./Core/Src/IRremote.d ./Core/Src/IRremote.o ./Core/Src/IRremote.su ./Core/Src/RGBLed.cyclo ./Core/Src/RGBLed.d ./Core/Src/RGBLed.o ./Core/Src/RGBLed.su ./Core/Src/Slot.cyclo ./Core/Src/Slot.d ./Core/Src/Slot.o ./Core/Src/Slot.su ./Core/Src/irSend.cyclo ./Core/Src/irSend.d ./Core/Src/irSend.o ./Core/Src/irSend.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/modbusSlave.cyclo ./Core/Src/modbusSlave.d ./Core/Src/modbusSlave.o ./Core/Src/modbusSlave.su ./Core/Src/modbus_crc.cyclo ./Core/Src/modbus_crc.d ./Core/Src/modbus_crc.o ./Core/Src/modbus_crc.su ./Core/Src/stm32g0xx_hal_msp.cyclo ./Core/Src/stm32g0xx_hal_msp.d ./Core/Src/stm32g0xx_hal_msp.o ./Core/Src/stm32g0xx_hal_msp.su ./Core/Src/stm32g0xx_it.cyclo ./Core/Src/stm32g0xx_it.d ./Core/Src/stm32g0xx_it.o ./Core/Src/stm32g0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g0xx.cyclo ./Core/Src/system_stm32g0xx.d ./Core/Src/system_stm32g0xx.o ./Core/Src/system_stm32g0xx.su

.PHONY: clean-Core-2f-Src

