################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
bmp180_task.obj: ../bmp180_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="bmp180_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cloud_task.obj: ../cloud_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="cloud_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

command_task.obj: ../command_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="command_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

compdcm_task.obj: ../compdcm_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="compdcm_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

isl29023_task.obj: ../isl29023_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="isl29023_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

senshub_iot.obj: ../senshub_iot.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="senshub_iot.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sht21_task.obj: ../sht21_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="sht21_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tmp006_task.obj: ../tmp006_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="tmp006_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


