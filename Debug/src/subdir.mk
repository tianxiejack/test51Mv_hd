################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../src/cuda.cu 

CPP_SRCS += \
../src/Displayer.cpp \
../src/Ipc.cpp \
../src/MultiChVideo.cpp \
../src/VideoProcess.cpp \
../src/app_ctrl.cpp \
../src/app_project_xgs026.cpp \
../src/cuda_mem.cpp \
../src/process51.cpp \
../src/test_videoProcess.cpp \
../src/v4l2camera.cpp 

OBJS += \
./src/Displayer.o \
./src/Ipc.o \
./src/MultiChVideo.o \
./src/VideoProcess.o \
./src/app_ctrl.o \
./src/app_project_xgs026.o \
./src/cuda.o \
./src/cuda_mem.o \
./src/process51.o \
./src/test_videoProcess.o \
./src/v4l2camera.o 

CU_DEPS += \
./src/cuda.d 

CPP_DEPS += \
./src/Displayer.d \
./src/Ipc.d \
./src/MultiChVideo.d \
./src/VideoProcess.d \
./src/app_ctrl.d \
./src/app_project_xgs026.d \
./src/cuda_mem.d \
./src/process51.d \
./src/test_videoProcess.d \
./src/v4l2camera.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=0 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_30,code=sm_30 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=0 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=0 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_30,code=sm_30 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=0 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I/usr/include/freetype2 -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp --compile --relocatable-device-code=false -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30 -m64 -ccbin aarch64-linux-gnu-g++  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


