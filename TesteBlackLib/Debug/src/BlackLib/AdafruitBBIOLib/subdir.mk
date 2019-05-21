################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BlackLib/AdafruitBBIOLib/c_adc.c \
../src/BlackLib/AdafruitBBIOLib/c_pinmux.c \
../src/BlackLib/AdafruitBBIOLib/c_pwm.c \
../src/BlackLib/AdafruitBBIOLib/c_uart.c \
../src/BlackLib/AdafruitBBIOLib/common.c \
../src/BlackLib/AdafruitBBIOLib/event_gpio.c 

CPP_SRCS += \
../src/BlackLib/AdafruitBBIOLib/bbio.cpp \
../src/BlackLib/AdafruitBBIOLib/gpio.cpp \
../src/BlackLib/AdafruitBBIOLib/pwm.cpp 

OBJS += \
./src/BlackLib/AdafruitBBIOLib/bbio.o \
./src/BlackLib/AdafruitBBIOLib/c_adc.o \
./src/BlackLib/AdafruitBBIOLib/c_pinmux.o \
./src/BlackLib/AdafruitBBIOLib/c_pwm.o \
./src/BlackLib/AdafruitBBIOLib/c_uart.o \
./src/BlackLib/AdafruitBBIOLib/common.o \
./src/BlackLib/AdafruitBBIOLib/event_gpio.o \
./src/BlackLib/AdafruitBBIOLib/gpio.o \
./src/BlackLib/AdafruitBBIOLib/pwm.o 

C_DEPS += \
./src/BlackLib/AdafruitBBIOLib/c_adc.d \
./src/BlackLib/AdafruitBBIOLib/c_pinmux.d \
./src/BlackLib/AdafruitBBIOLib/c_pwm.d \
./src/BlackLib/AdafruitBBIOLib/c_uart.d \
./src/BlackLib/AdafruitBBIOLib/common.d \
./src/BlackLib/AdafruitBBIOLib/event_gpio.d 

CPP_DEPS += \
./src/BlackLib/AdafruitBBIOLib/bbio.d \
./src/BlackLib/AdafruitBBIOLib/gpio.d \
./src/BlackLib/AdafruitBBIOLib/pwm.d 


# Each subdirectory must supply rules for building sources it contributes
src/BlackLib/AdafruitBBIOLib/%.o: ../src/BlackLib/AdafruitBBIOLib/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/BlackLib/AdafruitBBIOLib/%.o: ../src/BlackLib/AdafruitBBIOLib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


