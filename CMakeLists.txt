cmake_minimum_required(VERSION 3.15)
project(FIZZ C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(TARGET firmware)

file(GLOB_RECURSE C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE CXX_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ASM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s")

set(SOURCES ${C_FILES} ${CXX_FILES} ${ASM_FILES})

add_executable(${TARGET}.elf ${SOURCES})

target_include_directories(${TARGET}.elf PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CMSISHeaders
)

target_link_options(${TARGET}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/linker_STM32L476RGTX.ld
    -specs=./nosys.specs
    -nostdlib -nodefaultlibs -nostartfiles
    -Wl,--gc-sections
    -lgcc
)

add_custom_command(
    TARGET ${TARGET}.elf PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/nosys.specs
        ${CMAKE_BINARY_DIR}/nosys.specs
)

add_custom_command(
    TARGET ${TARGET}.elf PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/linker_STM32L476RGTX.ld
        ${CMAKE_BINARY_DIR}/linker_STM32L476RGTX.ld
)

add_custom_command(
    TARGET ${TARGET}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${TARGET}.elf ${TARGET}.hex
    COMMAND arm-none-eabi-size ${TARGET}.elf
)
