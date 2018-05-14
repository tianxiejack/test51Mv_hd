################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OSA_IPC/src/Ipcctl.cpp \
../src/OSA_IPC/src/Message.cpp \
../src/OSA_IPC/src/MessageQueueWrapper.cpp \
../src/OSA_IPC/src/SemaphoreWrapper.cpp \
../src/OSA_IPC/src/SharedMemoryWrapper.cpp \
../src/OSA_IPC/src/TimerWrapper.cpp 

OBJS += \
./src/OSA_IPC/src/Ipcctl.o \
./src/OSA_IPC/src/Message.o \
./src/OSA_IPC/src/MessageQueueWrapper.o \
./src/OSA_IPC/src/SemaphoreWrapper.o \
./src/OSA_IPC/src/SharedMemoryWrapper.o \
./src/OSA_IPC/src/TimerWrapper.o 

CPP_DEPS += \
./src/OSA_IPC/src/Ipcctl.d \
./src/OSA_IPC/src/Message.d \
./src/OSA_IPC/src/MessageQueueWrapper.d \
./src/OSA_IPC/src/SemaphoreWrapper.d \
./src/OSA_IPC/src/SharedMemoryWrapper.d \
./src/OSA_IPC/src/TimerWrapper.d 


# Each subdirectory must supply rules for building sources it contributes
src/OSA_IPC/src/%.o: ../src/OSA_IPC/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=1 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_30,code=sm_30 -m64 -odir "src/OSA_IPC/src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -D__IPC__=1 -D__MOVE_DETECT__=1 -D__TRACK__=1 -I/usr/lib/aarch64-linux-gnu/include -I../src/OSA_IPC/inc -I/usr/include/opencv -I/usr/include/opencv2 -I/usr/include/GL -I../include -I../include/APP -I../include/dxutc -I../src/OSA_CAP/inc -G -g -O0 -Xcompiler -fopenmp --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


