# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o Pico SDK
include(pico_sdk_import.cmake)

project(lampada_matriz C CXX ASM)

pico_sdk_init()

# Adiciona os arquivos do projeto
add_executable(lampada_matriz lampada_matriz.c inc/ssd1306_i2c.c)

pico_set_program_name(lampada_matriz "lampada_matriz")
pico_set_program_version(lampada_matriz "0.1")

# Gera cabeçalho do PIO
pico_generate_pio_header(lampada_matriz ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Habilita comunicação USB/UART
pico_enable_stdio_uart(lampada_matriz 0)
pico_enable_stdio_usb(lampada_matriz 1)

# Define diretórios de inclusão
target_include_directories(lampada_matriz PRIVATE inc)

# Linka bibliotecas necessárias
target_link_libraries(lampada_matriz pico_stdlib hardware_dma hardware_timer hardware_adc hardware_pio hardware_clocks hardware_i2c)

pico_add_extra_outputs(lampada_matriz)
