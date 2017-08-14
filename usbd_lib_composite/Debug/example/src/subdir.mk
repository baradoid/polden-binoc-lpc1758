################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cdc_vcom.c \
../example/src/composite_desc.c \
../example/src/composite_main.c \
../example/src/cr_startup_lpc175x_6x.c \
../example/src/hid_mouse.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cdc_vcom.o \
./example/src/composite_desc.o \
./example/src/composite_main.o \
./example/src/cr_startup_lpc175x_6x.o \
./example/src/hid_mouse.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cdc_vcom.d \
./example/src/composite_desc.d \
./example/src/composite_main.d \
./example/src/cr_startup_lpc175x_6x.d \
./example/src/hid_mouse.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_chip_175x_6x\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\usbd_lib_composite\example\inc" -I"C:\dev\polden-vert\polden-vert-motorControl-lpc1758\lpc_chip_175x_6x\inc\usbd" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


