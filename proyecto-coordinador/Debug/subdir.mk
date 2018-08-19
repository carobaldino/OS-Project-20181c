################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../algoritmos_distribucion.c \
../conexiones_coordinador.c \
../config_coordinador.c \
../coordinador.c 

OBJS += \
./algoritmos_distribucion.o \
./conexiones_coordinador.o \
./config_coordinador.o \
./coordinador.o 

C_DEPS += \
./algoritmos_distribucion.d \
./conexiones_coordinador.d \
./config_coordinador.d \
./coordinador.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

coordinador.o: ../coordinador.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"coordinador.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


