################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cargarCasos.cpp \
../main.cpp \
../main2.cpp \
../main3.cpp \
../red.cpp \
../similitud.cpp \
../str_util.cpp 

OBJS += \
./cargarCasos.o \
./main.o \
./main2.o \
./main3.o \
./red.o \
./similitud.o \
./str_util.o 

CPP_DEPS += \
./cargarCasos.d \
./main.d \
./main2.d \
./main3.d \
./red.d \
./similitud.d \
./str_util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	mpicxx -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


