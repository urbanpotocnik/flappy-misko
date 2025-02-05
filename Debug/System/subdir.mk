################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../System/LED.c \
../System/SCI.c \
../System/XPT2046_touch.c \
../System/buf.c \
../System/joystick.c \
../System/kbd.c \
../System/lcd.c \
../System/lcd_backlight.c \
../System/lcd_ili9341.c \
../System/periodic_services.c \
../System/timing_utils.c \
../System/ugui.c 

OBJS += \
./System/LED.o \
./System/SCI.o \
./System/XPT2046_touch.o \
./System/buf.o \
./System/joystick.o \
./System/kbd.o \
./System/lcd.o \
./System/lcd_backlight.o \
./System/lcd_ili9341.o \
./System/periodic_services.o \
./System/timing_utils.o \
./System/ugui.o 

C_DEPS += \
./System/LED.d \
./System/SCI.d \
./System/XPT2046_touch.d \
./System/buf.d \
./System/joystick.d \
./System/kbd.d \
./System/lcd.d \
./System/lcd_backlight.d \
./System/lcd_ili9341.d \
./System/periodic_services.d \
./System/timing_utils.d \
./System/ugui.d 


# Each subdirectory must supply rules for building sources it contributes
System/%.o System/%.su System/%.cyclo: ../System/%.c System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Tomaz/Desktop/New folder/flappy-misko/System/Include" -I"C:/Users/Tomaz/Desktop/New folder/flappy-misko/Application/Include" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-System

clean-System:
	-$(RM) ./System/LED.cyclo ./System/LED.d ./System/LED.o ./System/LED.su ./System/SCI.cyclo ./System/SCI.d ./System/SCI.o ./System/SCI.su ./System/XPT2046_touch.cyclo ./System/XPT2046_touch.d ./System/XPT2046_touch.o ./System/XPT2046_touch.su ./System/buf.cyclo ./System/buf.d ./System/buf.o ./System/buf.su ./System/joystick.cyclo ./System/joystick.d ./System/joystick.o ./System/joystick.su ./System/kbd.cyclo ./System/kbd.d ./System/kbd.o ./System/kbd.su ./System/lcd.cyclo ./System/lcd.d ./System/lcd.o ./System/lcd.su ./System/lcd_backlight.cyclo ./System/lcd_backlight.d ./System/lcd_backlight.o ./System/lcd_backlight.su ./System/lcd_ili9341.cyclo ./System/lcd_ili9341.d ./System/lcd_ili9341.o ./System/lcd_ili9341.su ./System/periodic_services.cyclo ./System/periodic_services.d ./System/periodic_services.o ./System/periodic_services.su ./System/timing_utils.cyclo ./System/timing_utils.d ./System/timing_utils.o ./System/timing_utils.su ./System/ugui.cyclo ./System/ugui.d ./System/ugui.o ./System/ugui.su

.PHONY: clean-System

