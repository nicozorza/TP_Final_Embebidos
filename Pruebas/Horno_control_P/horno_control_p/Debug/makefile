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
all: horno_control_p.axf

# Tool invocations
horno_control_p.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/lpc_chip_43xx/Debug" -L"/home/nicozorza/LPCXpresso/TP_Final/Pruebas/Horno_control_P/lpc_board_nxp_lpcxpresso_4337/Debug" -Xlinker -Map="horno_control_p.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "freertos_examples_1_to_9_Debug.ld" -o "horno_control_p.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) horno_control_p.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "horno_control_p.axf"; # arm-none-eabi-objcopy -v -O binary "horno_control_p.axf" "horno_control_p.bin" ; # checksum -p LPC4337 -d "horno_control_p.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
