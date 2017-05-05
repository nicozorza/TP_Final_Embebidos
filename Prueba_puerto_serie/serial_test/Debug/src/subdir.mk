################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_start_m0.c \
../src/cr_startup_lpc43xx.c \
../src/crp.c \
../src/serial_test.c \
../src/sysinit.c 

OBJS += \
./src/cr_start_m0.o \
./src/cr_startup_lpc43xx.o \
./src/crp.o \
./src/serial_test.o \
./src/sysinit.o 

C_DEPS += \
./src/cr_start_m0.d \
./src/cr_startup_lpc43xx.d \
./src/crp.d \
./src/serial_test.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__MULTICORE_NONE -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_LPCOPEN -D__LPC43XX__ -D__REDLIB__ -I"/home/nicozorza/LPCXpresso/TP_Final/Prueba_puerto_serie/lpc_board_nxp_lpcxpresso_4337/inc" -I"/home/nicozorza/LPCXpresso/TP_Final/Prueba_puerto_serie/lpc_chip_43xx/inc" -I"/home/nicozorza/LPCXpresso/TP_Final/Prueba_puerto_serie/lpc_chip_43xx/inc/usbd" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


