################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/GF68_8051_main.c 

OBJS += \
./src/GF68_8051_main.OBJ \
./src/SILABS_STARTUP.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/GF68_8051_main.OBJ: D:/8051_mcu_ide/install/developer/sdks/8051/v4.1.5/Device/C8051F300/inc/SI_C8051F300_Register_Enums.h D:/8051_mcu_ide/install/developer/sdks/8051/v4.1.5/Device/C8051F300/inc/SI_C8051F300_Defs.h D:/8051_mcu_ide/install/developer/sdks/8051/v4.1.5/Device/shared/si8051Base/compiler_defs.h D:/8051_mcu_ide/install/developer/sdks/8051/v4.1.5/Device/shared/si8051Base/stdbool.h D:/8051_mcu_ide/install/developer/sdks/8051/v4.1.5/Device/shared/si8051Base/stdint.h

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	AX51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '


