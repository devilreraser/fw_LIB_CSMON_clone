################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
device/%.obj: ../device/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O2 --include_path="C:/ti/c2000/C2000Ware_2_00_00_02/driverlib/f2837xd/driverlib" --include_path="D:/repo_git/fw_LIB_CSMON/05_Project/TEST_CSMON_C28xx" --include_path="D:/repo_git/fw_LIB_CSMON/01_Source_Test" --include_path="D:/repo_git/fw_LIB_CSMON/05_Project/TEST_CSMON_C28xx/device" --include_path="D:/repo_git/fw_LIB_CSMON/02_Libraries/ModbusAddress/01_Source" --include_path="C:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --define=_FLASH --define=CPU1 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="device/$(basename $(<F)).d_raw" --obj_directory="device" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

device/%.obj: ../device/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O2 --include_path="C:/ti/c2000/C2000Ware_2_00_00_02/driverlib/f2837xd/driverlib" --include_path="D:/repo_git/fw_LIB_CSMON/05_Project/TEST_CSMON_C28xx" --include_path="D:/repo_git/fw_LIB_CSMON/01_Source_Test" --include_path="D:/repo_git/fw_LIB_CSMON/05_Project/TEST_CSMON_C28xx/device" --include_path="D:/repo_git/fw_LIB_CSMON/02_Libraries/ModbusAddress/01_Source" --include_path="C:/ti/ccs910/ccs/tools/compiler/ti-cgt-c2000_18.12.2.LTS/include" --define=_FLASH --define=CPU1 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="device/$(basename $(<F)).d_raw" --obj_directory="device" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


