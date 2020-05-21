################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BootLoader.c \
../src/DGPIO.c \
../src/DMA.c \
../src/DMA_Cfg.c \
../src/DNVIC.c \
../src/DRCC.c \
../src/FLITF.c \
../src/HUART.c \
../src/UART.c 

OBJS += \
./src/BootLoader.o \
./src/DGPIO.o \
./src/DMA.o \
./src/DMA_Cfg.o \
./src/DNVIC.o \
./src/DRCC.o \
./src/FLITF.o \
./src/HUART.o \
./src/UART.o 

C_DEPS += \
./src/BootLoader.d \
./src/DGPIO.d \
./src/DMA.d \
./src/DMA_Cfg.d \
./src/DNVIC.d \
./src/DRCC.d \
./src/FLITF.d \
./src/HUART.d \
./src/UART.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


