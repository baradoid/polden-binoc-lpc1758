################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: periph_rtc.axf

# Tool invocations
periph_rtc.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_chip_175x_6x\Debug" -L"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_board_nxp_lpcxpresso_1769\Debug" -Xlinker -Map="periph_rtc.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "periph_rtc_Debug.ld" -o "periph_rtc.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) periph_rtc.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "periph_rtc.axf"; # arm-none-eabi-objcopy -v -O binary "periph_rtc.axf" "periph_rtc.bin" ; # checksum -p LPC1769 -d "periph_rtc.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
