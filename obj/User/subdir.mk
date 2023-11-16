################################################################################
# MRS Version: 1.9.0
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32x035_it.c \
../User/hw_borad.c \
../User/key.c \
../User/main.c \
../User/system_ch32x035.c 

OBJS += \
./User/ch32x035_it.o \
./User/hw_borad.o \
./User/key.o \
./User/main.o \
./User/system_ch32x035.o 

C_DEPS += \
./User/ch32x035_it.d \
./User/hw_borad.d \
./User/key.d \
./User/main.d \
./User/system_ch32x035.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"D:\项目资料\K001\工程\CH32X035G8U6\Debug" -I"../inc" -I"D:\项目资料\K001\工程\CH32X035G8U6\Core" -I"D:\项目资料\K001\工程\CH32X035G8U6\User" -I"D:\项目资料\K001\工程\CH32X035G8U6\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

