################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
$(ROOT)/TestSCA.cpp 

OBJS += \
./TestSCA.o 

DEPS += \
${addprefix ./, \
TestSCA.d \
}


# Each subdirectory must supply rules for building sources it contributes
%.o: $(ROOT)/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@echo g++ -D_DEBUG -I${SCA4CPP_HOME}/runtime/core/src -I${SDO4CPP}/include -I${SCA4CPP_HOME}/runtime/core/test/MyValue -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $<
	@g++ -D_DEBUG -I${SCA4CPP_HOME}/runtime/core/src -I${SDO4CPP}/include -I${SCA4CPP_HOME}/runtime/core/test/MyValue -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
	echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	g++ -MM -MG -P -w -D_DEBUG -I${SCA4CPP_HOME}/runtime/core/src -I${SDO4CPP}/include -I${SCA4CPP_HOME}/runtime/core/test/MyValue -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


