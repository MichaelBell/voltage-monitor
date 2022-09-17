cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME voltage-monitor) # <-- Name your project/executable here!

include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    voltagemon.cpp
)

pico_enable_stdio_uart(${NAME} 0)
pico_enable_stdio_usb(${NAME} 1)

# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project
include(common/pimoroni_bus)
include(drivers/st7789/st7789)
include(libraries/pico_graphics/pico_graphics)
include(libraries/bitmap_fonts/bitmap_fonts)
include(libraries/hershey_fonts/hershey_fonts)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
  st7789
  pico_graphics
  hardware_adc
  pico_multicore
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
