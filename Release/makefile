################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include src/OSD_CR/subdir.mk
-include src/OSD/subdir.mk
-include src/OSA_CAP/src/subdir.mk
-include src/MSGDRI/subdir.mk
-include src/LASERBLOB/subdir.mk
-include src/DXUTC/subdir.mk
-include src/APP/subdir.mk

OS_SUFFIX := $(subst Linux,linux,$(subst Darwin/x86_64,darwin,$(shell uname -s)/$(shell uname -m)))

-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CU_DEPS)),)
-include $(CU_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# OpenGL+GLUT OS-specific define
ifeq ($(shell uname -s),Darwin)
GLUT_LIBS := -Xlinker -framework -Xlinker GLUT -Xlinker -framework -Xlinker OpenGL
else
GLUT_LIBS := -lGL -lGLU -lglut 
endif


# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: test51Mv_hd

# Tool invocations
test51Mv_hd: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: NVCC Linker'
	/usr/local/cuda-8.0/bin/nvcc --cudart static -L/usr/lib/aarch64-linux-gnu -L/usr/lib/aarch64-linux-gnu/tegra -L/usr/local/cuda-8.0/targets/aarch64-linux-gnu/lib -Xlinker -rpath=/usr/lib/aarch64-linux-gnu/tegra --relocatable-device-code=false -gencode arch=compute_20,code=compute_20 -gencode arch=compute_20,code=sm_20 -m64 -ccbin aarch64-linux-gnu-g++ -link -o  "test51Mv_hd" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CU_DEPS)$(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) test51Mv_hd
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
