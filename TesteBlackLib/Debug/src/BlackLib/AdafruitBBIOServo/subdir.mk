################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BlackLib/AdafruitBBIOServo/AdafruitBBIOServo.cpp 

OBJS += \
./src/BlackLib/AdafruitBBIOServo/AdafruitBBIOServo.o 

CPP_DEPS += \
./src/BlackLib/AdafruitBBIOServo/AdafruitBBIOServo.d 


# Each subdirectory must supply rules for building sources it contributes
src/BlackLib/AdafruitBBIOServo/%.o: ../src/BlackLib/AdafruitBBIOServo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


