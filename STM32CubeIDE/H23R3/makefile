################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Thirdparty/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Thirdparty/Middleware/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0/subdir.mk
-include Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Thirdparty/Middleware/FreeRTOS/Source/subdir.mk
-include Thirdparty/CMSIS/Device/ST/STM32F0xx/Source/Templates/subdir.mk
-include H23R3x/subdir.mk
-include BOS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := H23R3x
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
H23R3x.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
H23R3x.list \

OBJCOPY_BIN += \
H23R3x.bin \


# All Target
all: main-build

# Main-build Target
main-build: H23R3x.elf secondary-outputs

# Tool invocations
H23R3x.elf: $(OBJS) $(USER_OBJS) D:\Hexabitz\for\ Release\Modules\ firmware\H23R3x\STM32CubeIDE\STM32F091RCTX_FLASH.ld
	arm-none-eabi-gcc -o "H23R3x.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"D:\Hexabitz\for Release\Modules firmware\H23R3x\STM32CubeIDE\STM32F091RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="H23R3x.map" -Wl,--gc-sections -static  -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

H23R3x.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "H23R3x.list"
	@echo 'Finished building: $@'
	@echo ' '

H23R3x.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "H23R3x.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
