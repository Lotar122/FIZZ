cmake_minimum_required(VERSION 3.15)
project(FIZZ C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Your target name
set(TARGET firmware)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "")

file(GLOB_RECURSE C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE CXX_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ASM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s")

# Source files
set(SOURCES
    ${C_FILES}
    ${CXX_FILES}
    ${ASM_FILES}
)

add_executable(${TARGET}.elf ${SOURCES})

target_include_directories(${TARGET}.elf PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CMSISHeaders
)

# Linker script
target_link_options(${TARGET}.elf PRIVATE -T${CMAKE_SOURCE_DIR}/linker_STM32L476RGTX.ld -specs=./nosys.specs)

# Remove all implicit libs
target_link_options(${TARGET}.elf PRIVATE -nostdlib -nodefaultlibs -nostartfiles -Wl,--gc-sections)

# Optional: Keep libgcc for compiler helpers
target_link_options(${TARGET}.elf PRIVATE -lgcc)

add_custom_command(
    TARGET ${TARGET}.elf
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/nosys.specs
            ${CMAKE_BINARY_DIR}/nosys.specs
)

add_custom_command(
    TARGET ${TARGET}.elf
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/linker_STM32L476RGTX.ld
            ${CMAKE_BINARY_DIR}/linker_STM32L476RGTX.ld
)

# Output formats
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${TARGET}.elf ${TARGET}.bin
    COMMAND arm-none-eabi-size ${TARGET}.elf
)
