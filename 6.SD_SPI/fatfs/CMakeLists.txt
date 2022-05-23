set(PICO_SDK_FATFS_SOURCE .)

add_library(fatfs
    ${PICO_SDK_FATFS_SOURCE}/src/ff.c
    ${PICO_SDK_FATFS_SOURCE}/src/ffsystem.c
    ${PICO_SDK_FATFS_SOURCE}/src/ffunicode.c
    ${PICO_SDK_FATFS_SOURCE}/src/tf_card.c
)

target_include_directories(fatfs PUBLIC
    .
    ${PICO_SDK_FATFS_SOURCE}/include
)

target_link_libraries(fatfs
    pico_stdlib 
    hardware_clocks 
    hardware_spi
)


