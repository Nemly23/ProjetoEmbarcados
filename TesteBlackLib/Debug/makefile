################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/BlackLib/examples/subdir.mk
-include src/BlackLib/BlackUART/subdir.mk
-include src/BlackLib/BlackTime/subdir.mk
-include src/BlackLib/BlackThread/subdir.mk
-include src/BlackLib/BlackServo/subdir.mk
-include src/BlackLib/BlackSPI/subdir.mk
-include src/BlackLib/BlackPWM/subdir.mk
-include src/BlackLib/BlackMutex/subdir.mk
-include src/BlackLib/BlackI2C/subdir.mk
-include src/BlackLib/BlackGPIO/subdir.mk
-include src/BlackLib/BlackDirectory/subdir.mk
-include src/BlackLib/BlackADC/subdir.mk
-include src/BlackLib/AdafruitBBIOServo/subdir.mk
-include src/BlackLib/AdafruitBBIOLib/subdir.mk
-include src/BlackLib/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TesteBlackLib

# Tool invocations
TesteBlackLib: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-linux-gnueabihf-g++  -o "TesteBlackLib" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) TesteBlackLib
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
