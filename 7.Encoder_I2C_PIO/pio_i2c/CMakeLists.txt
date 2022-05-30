set(PIO_I2C_SOURCE .)

# add_library(freeact
#    ${PIO_I2C_SOURCE}/pio_i2c.c
# )

add_library(pio_i2c
    pio_i2c.c
)

target_include_directories(pio_i2c PUBLIC
    .
)

target_link_libraries(pio_i2c 
    pico_stdlib 
    hardware_pio
)

pico_generate_pio_header(pio_i2c ${CMAKE_CURRENT_LIST_DIR}/i2c.pio)



