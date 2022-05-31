cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(Pico)

pico_sdk_init()


add_subdirectory(ProjectFiles)
add_subdirectory(pio_stepper)