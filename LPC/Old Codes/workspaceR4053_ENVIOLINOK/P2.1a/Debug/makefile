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
all: P2.1a.axf

# Tool invocations
P2.1a.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Eugenia\Desktop\workspaceR4053\CMSISv1p30_LPC17xx\Debug" -L"C:\Users\Eugenia\Desktop\workspaceR4053\Lib_CMSIS\Debug" -Xlinker -Map=P2.1a.map -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "Blinky_Debug.ld" -o "P2.1a.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) P2.1a.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size P2.1a.axf; # arm-none-eabi-objcopy -O binary P2.1a.axf P2.1a.bin ; checksum -p LPC1769 -d P2.1a.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
