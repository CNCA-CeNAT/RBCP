################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Servidor/servidor.cpp \
../Servidor/socket.cpp 

OBJS += \
./Servidor/servidor.o \
./Servidor/socket.o 

CPP_DEPS += \
./Servidor/servidor.d \
./Servidor/socket.d 


# Each subdirectory must supply rules for building sources it contributes
Servidor/%.o: ../Servidor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	mpicxx -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


