add_executable(test_main)

target_sources(test_main PRIVATE test_main.cpp PWM_IO.cpp)

target_link_libraries(test_main PRIVATE 
pico_stdlib 
hardware_gpio
hardware_spi
hardware_uart)

pico_add_extra_outputs(test_main)

pico_enable_stdio_usb(test_main 1)

# add url via pico_set_program_url
example_auto_set_url(test_main)