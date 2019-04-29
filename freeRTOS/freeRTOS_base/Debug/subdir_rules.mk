################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base/drivers" --include_path="C:/ti/tivaware_c_series_2_1_4_178/inc" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base" --include_path="C:/ti/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c1294xl" --include_path="G:/My Drive/CUB Study" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base/third_party" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base/third_party/FreeRTOS/Source/include" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="G:/My Drive/CUB Study/Semester 1/ESD/ECEN5013_AESD-S19/Project2/freeRTOS_base/third_party/FreeRTOS" --advice:power=all --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=SNOWFLAKE --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


