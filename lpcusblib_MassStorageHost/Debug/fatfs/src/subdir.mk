################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fatfs/src/ff.c \
../fatfs/src/fs_usb.c \
../fatfs/src/rtc.c 

OBJS += \
./fatfs/src/ff.o \
./fatfs/src/fs_usb.o \
./fatfs/src/rtc.o 

C_DEPS += \
./fatfs/src/ff.d \
./fatfs/src/fs_usb.d \
./fatfs/src/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
fatfs/src/%.o: ../fatfs/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__LPC175X_6X__ -DUSB_HOST_ONLY -DCORE_M3 -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_chip_175x_6x\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpcusblib_MassStorageHost\example\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpcusblib_MassStorageHost\lpcusblib\Drivers\USB" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpcusblib_MassStorageHost\fatfs\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


