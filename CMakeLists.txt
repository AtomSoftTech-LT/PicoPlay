# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/VSARM/sdk/pico/pico-sdk")


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(blinkSPI C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(blinkSPI blinkSPI.c )

FILE(GLOB FreeRTOS_src FreeRTOS-Kernel/*.c)

add_library( FreeRTOS STATIC
    ${FreeRTOS_src}
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    FreeRTOS-Kernel/portable/MemMang/heap_4.c 
)

pico_set_program_name(blinkSPI "blinkSPI")
pico_set_program_version(blinkSPI "0.1")

pico_enable_stdio_uart(blinkSPI 0)
pico_enable_stdio_usb(blinkSPI 1)

target_include_directories( FreeRTOS PUBLIC
    FreeRTOS-Kernel/include
    /include
    FreeRTOS-Kernel/portable/GCC/ARM_CM0
)
# Add the standard library to the build
target_link_libraries(blinkSPI 
    pico_stdlib
    hardware_gpio
    hardware_spi
    FreeRTOS
)

pico_add_extra_outputs(blinkSPI)

