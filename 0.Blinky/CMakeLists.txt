# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.19)

# Include build functions from Pico SDK
#include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(blinky C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    src/main.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Include FreeRTOS implementation
FILE(GLOB FreeRTOS_src FreeRTOS-Kernel/*.c)

add_library( FreeRTOS STATIC
	${FreeRTOS_src}
	FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
	FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

# Include FreeRTOS .h

target_include_directories( FreeRTOS PUBLIC
		FreeRTOS-Kernel/include
		include/
		FreeRTOS-Kernel/portable/GCC/ARM_CM0
	)


# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    FreeRTOS
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)