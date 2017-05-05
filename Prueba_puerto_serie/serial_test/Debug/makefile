################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: serial_test.axf

# Tool invocations
serial_test.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/nicozorza/LPCXpresso/TP_Final/Prueba_puerto_serie/lpc_board_nxp_lpcxpresso_4337/Debug" -L"/home/nicozorza/LPCXpresso/TP_Final/Prueba_puerto_serie/lpc_chip_43xx/Debug" -Xlinker -Map="serial_test.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "serial_test_Debug.ld" -o "serial_test.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) serial_test.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "serial_test.axf"; # arm-none-eabi-objcopy -v -O binary "serial_test.axf" "serial_test.bin" ; # checksum -p LPC4337 -d "serial_test.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
