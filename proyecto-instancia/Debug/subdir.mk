################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../config_instancia.c \
../consola-instancia.c \
../get_set_store.c \
../instancia.c 

OBJS += \
./config_instancia.o \
./consola-instancia.o \
./get_set_store.o \
./instancia.o 

C_DEPS += \
./config_instancia.d \
./consola-instancia.d \
./get_set_store.d \
./instancia.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


