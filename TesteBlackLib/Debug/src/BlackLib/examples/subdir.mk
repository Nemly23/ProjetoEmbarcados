################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BlackLib/examples/ExampleMain.cpp 

OBJS += \
./src/BlackLib/examples/ExampleMain.o 

CPP_DEPS += \
./src/BlackLib/examples/ExampleMain.d 


# Each subdirectory must supply rules for building sources it contributes
src/BlackLib/examples/%.o: ../src/BlackLib/examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


