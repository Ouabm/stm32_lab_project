set(STARTUP_FILE ${PROJECT_SOURCE_DIR}/main/include/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h743xx.s)
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/main/include/STM32H743XX_FLASH.ld)

# Créer l'exécutable
add_executable(stm32_rt
    ${PROJECT_SOURCE_DIR}/main/main.cpp
    ${STARTUP_FILE}
)
target_sources(stm32_rt PRIVATE
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c 
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c 
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/main.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/tim.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/gpio.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/adc.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/system_stm32h7xx.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/stm32h7xx_hal_msp.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/system_stm32h7xx.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/syscalls.c
    ${PROJECT_SOURCE_DIR}/main/include/Core/Src/sysmem.c
    ${PROJECT_SOURCE_DIR}/main/include/DHT_11/DHT.c
 
)

# Ajouter les include paths
target_include_directories(stm32_rt PUBLIC
    
    ${PROJECT_SOURCE_DIR}/Inc
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/CMSIS/Device/ST/STM32H7xx/Include
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/CMSIS/Include
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/BSP/STM32H7xx_Nucleo
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Inc
    ${PROJECT_SOURCE_DIR}/main/include/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h
    ${PROJECT_SOURCE_DIR}/main/include/Core/Inc
    ${PROJECT_SOURCE_DIR}/main/include
    ${PROJECT_SOURCE_DIR}/main/include/USB_DEVICE/App
    ${PROJECT_SOURCE_DIR}/main/include/USB_DEVICE/Target
    ${PROJECT_SOURCE_DIR}/main/include/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJECT_SOURCE_DIR}/main/include/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJECT_SOURCE_DIR}/main/include/DHT_11
    $ENV{CADMIUM}
)

# Définitions
target_compile_definitions(stm32_rt PRIVATE STM32H743xx)

# Options de compilation
target_compile_options(stm32_rt PRIVATE
    -mcpu=cortex-m7
    -mthumb
    -fexceptions
    -Wno-unused-parameter
    -Wno-psabi
     $<$<COMPILE_LANGUAGE:CXX>:-frtti>
)

# Options de linkage
target_link_options(stm32_rt PRIVATE
    "-T${LINKER_SCRIPT}"
    -mcpu=cortex-m7
    -mthumb
    -Wl,--gc-sections
)



