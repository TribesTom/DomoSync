################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
third_party/FreeRTOS/Source/portable/MemMang/heap_1.obj: C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/MemMang/heap_1.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.9/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.9/include" --include_path="E:/DomoSync/workspace/senshub_iot" --include_path="C:/ti/TivaWareC/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWareC" --include_path="C:/ti/TivaWareC/third_party" --include_path="C:/ti/TivaWareC/third_party/exosite" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWareC/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWareC/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="third_party/FreeRTOS/Source/portable/MemMang/heap_1.pp" --obj_directory="third_party/FreeRTOS/Source/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


