################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MiscFunctions/Shapes/src/%.obj: ../MiscFunctions/Shapes/src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/diamond/ti/ccs1240/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=0 --include_path="/home/diamond/UP2/SW-TM4C-2.2.0.295" --include_path="/home/diamond/UP2/RTOS" --include_path="/home/diamond/ti/ccs1240/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="MiscFunctions/Shapes/src/$(basename $(<F)).d_raw" --obj_directory="MiscFunctions/Shapes/src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


