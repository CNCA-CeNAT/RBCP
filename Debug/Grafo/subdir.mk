################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../Grafo/caso.o 

CPP_SRCS += \
../Grafo/caso.cpp \
../Grafo/grafo.cpp \
../Grafo/indice.cpp \
../Grafo/lista_av.cpp \
../Grafo/lista_nodos.cpp \
../Grafo/nodo.cpp \
../Grafo/norma.cpp \
../Grafo/parser.cpp \
../Grafo/pila.cpp 

OBJS += \
./Grafo/caso.o \
./Grafo/grafo.o \
./Grafo/indice.o \
./Grafo/lista_av.o \
./Grafo/lista_nodos.o \
./Grafo/nodo.o \
./Grafo/norma.o \
./Grafo/parser.o \
./Grafo/pila.o 

CPP_DEPS += \
./Grafo/caso.d \
./Grafo/grafo.d \
./Grafo/indice.d \
./Grafo/lista_av.d \
./Grafo/lista_nodos.d \
./Grafo/nodo.d \
./Grafo/norma.d \
./Grafo/parser.d \
./Grafo/pila.d 


# Each subdirectory must supply rules for building sources it contributes
Grafo/%.o: ../Grafo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	mpicxx -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


