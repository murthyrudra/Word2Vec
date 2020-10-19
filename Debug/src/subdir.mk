################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ActivationFunction.cpp \
../src/TrainSkipGram.cpp \
../src/WordEmbeddingCbowNS.cpp \
../src/loss.cpp \
../src/mathematicalUtility.cpp \
../src/utility.cpp 

C_SRCS += \
../src/gnuplot_i.c 

OBJS += \
./src/ActivationFunction.o \
./src/TrainSkipGram.o \
./src/WordEmbeddingCbowNS.o \
./src/gnuplot_i.o \
./src/loss.o \
./src/mathematicalUtility.o \
./src/utility.o 

CPP_DEPS += \
./src/ActivationFunction.d \
./src/TrainSkipGram.d \
./src/WordEmbeddingCbowNS.d \
./src/loss.d \
./src/mathematicalUtility.d \
./src/utility.d 

C_DEPS += \
./src/gnuplot_i.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I/Users/rudra/Software/boost_1_73_0/ -O2 -g1 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/Users/rudra/Software/boost_1_73_0/ -O2 -g1 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


