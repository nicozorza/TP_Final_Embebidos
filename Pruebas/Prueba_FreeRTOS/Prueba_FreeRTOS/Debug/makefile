################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include freertos/src/subdir.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Prueba_FreeRTOS.axf

# Tool invocations
Prueba_FreeRTOS.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Prueba_FreeRTOS/lpc_chip_43xx/Debug" -L"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Prueba_FreeRTOS/lpc_board_nxp_lpcxpresso_4337/Debug" -Xlinker -Map="Prueba_FreeRTOS.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "Prueba_FreeRTOS_Debug.ld" -o "Prueba_FreeRTOS.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Prueba_FreeRTOS.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Prueba_FreeRTOS.axf"; # arm-none-eabi-objcopy -v -O binary "Prueba_FreeRTOS.axf" "Prueba_FreeRTOS.bin" ; # checksum -p LPC4337 -d "Prueba_FreeRTOS.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
