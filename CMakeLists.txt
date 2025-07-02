cmake_minimum_required(VERSION 3.22)
message(STATUS "BUILD STM32")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake" CACHE STRING "")

project(stm32_rt C CXX ASM)

# Définir les standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m7 -mthumb -fno-exceptions -frtti")


# Répertoire de sortie pour les binaires
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Ajouter les sous-dossiers
add_subdirectory(main)
#add_subdirectory(cmake/stm32cubemx)