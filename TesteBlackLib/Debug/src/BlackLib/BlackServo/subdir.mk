################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BlackLib/BlackServo/BlackServo.cpp 

OBJS += \
./src/BlackLib/BlackServo/BlackServo.o 

CPP_DEPS += \
./src/BlackLib/BlackServo/BlackServo.d 


# Each subdirectory must supply rules for building sources it contributes
src/BlackLib/BlackServo/%.o: ../src/BlackLib/BlackServo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


