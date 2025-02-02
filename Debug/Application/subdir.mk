################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/game.c \
../Application/graphics.c \
../Application/math_utils.c \
../Application/objects.c 

OBJS += \
./Application/game.o \
./Application/graphics.o \
./Application/math_utils.o \
./Application/objects.o 

C_DEPS += \
./Application/game.d \
./Application/graphics.d \
./Application/math_utils.d \
./Application/objects.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o Application/%.su Application/%.cyclo: ../Application/%.c Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Tomaz/Desktop/New folder/flappy-misko/System/Include" -I"C:/Users/Tomaz/Desktop/New folder/flappy-misko/Application/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/game.cyclo ./Application/game.d ./Application/game.o ./Application/game.su ./Application/graphics.cyclo ./Application/graphics.d ./Application/graphics.o ./Application/graphics.su ./Application/math_utils.cyclo ./Application/math_utils.d ./Application/math_utils.o ./Application/math_utils.su ./Application/objects.cyclo ./Application/objects.d ./Application/objects.o ./Application/objects.su

.PHONY: clean-Application

