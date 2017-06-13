################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/Configure.c \
../example/src/cr_startup_lpc43xx.c \
../example/src/main.c \
../example/src/sysinit.c \
../example/src/utils.c 

OBJS += \
./example/src/Configure.o \
./example/src/cr_startup_lpc43xx.o \
./example/src/main.o \
./example/src/sysinit.o \
./example/src/utils.o 

C_DEPS += \
./example/src/Configure.d \
./example/src/cr_startup_lpc43xx.d \
./example/src/main.d \
./example/src/sysinit.d \
./example/src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__MULTICORE_NONE -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M4 -I"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/lpc_chip_43xx/inc" -I"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/lpc_board_nxp_lpcxpresso_4337/inc" -I"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/horno_control_p/example/inc" -I"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/horno_control_p/freertos/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


