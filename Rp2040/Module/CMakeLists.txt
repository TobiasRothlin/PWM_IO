add_executable(pwm_io_main)

# by default the header is generated into the build dir
pico_generate_pio_header(pwm_io_main ${CMAKE_CURRENT_LIST_DIR}/pio_pwm_in.pio)
# however, alternatively you can choose to generate it somewhere else (in this case in the source tree for check in)
#pico_generate_pio_header(pio_blink ${CMAKE_CURRENT_LIST_DIR}/blink.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})

target_sources(pwm_io_main PRIVATE pwm_io_main.cpp pwm_in.cpp pwm_out.cpp)

target_link_libraries(pwm_io_main PRIVATE 
pico_stdlib 
hardware_pio
hardware_pwm
hardware_gpio
hardware_spi
hardware_uart)

pico_add_extra_outputs(pwm_io_main)

pico_enable_stdio_usb(pwm_io_main 1)

# add url via pico_set_program_url
example_auto_set_url(pwm_io_main)