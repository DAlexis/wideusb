cmake_minimum_required(VERSION 3.8)

project(wideusb C CXX ASM)

IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)
    message(FATAL_ERROR "Toolchain arm-none-eabi not selected! Please run cmake with -DCMAKE_TOOLCHAIN_FILE=stm32f407.cmake")
ENDIF()

set(H_HEADERS
    ${PROJECT_SOURCE_DIR}/Core/Inc/FreeRTOSConfig.h
    ${PROJECT_SOURCE_DIR}/Core/Inc/gpio.h
    ${PROJECT_SOURCE_DIR}/Core/Inc/main.h
    ${PROJECT_SOURCE_DIR}/Core/Inc/stm32f4xx_hal_conf.h
    ${PROJECT_SOURCE_DIR}/Core/Inc/stm32f4xx_it.h

    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/arm_common_tables.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/arm_const_structs.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/arm_math.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/cmsis_armcc.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/cmsis_armcc_V6.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/cmsis_gcc.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cm0.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cm0plus.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cm3.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cm4.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cm7.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cmFunc.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cmInstr.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_cmSimd.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_sc000.h
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include/core_sc300.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ramfunc.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_usb.h

    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/usbd_cdc.h
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_core.h
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ctlreq.h
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_def.h
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ioreq.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/croutine.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/event_groups.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/list.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/mpu_prototypes.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/mpu_wrappers.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/portable.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/projdefs.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/queue.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/semphr.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/StackMacros.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/task.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/timers.h
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/portmacro.h

    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usbd_cdc_if.h
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usbd_desc.h
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usb_device.h
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/Target/usbd_conf.h

    ${PROJECT_SOURCE_DIR}/cpp/include/cpp-bootstrap.h
    ${PROJECT_SOURCE_DIR}/cpp/include/os/cpp-freertos.hpp
)

set(C_SOURCES
    ${PROJECT_SOURCE_DIR}/Core/Src/freertos.c
    ${PROJECT_SOURCE_DIR}/Core/Src/gpio.c
    ${PROJECT_SOURCE_DIR}/Core/Src/main.c
    ${PROJECT_SOURCE_DIR}/Core/Src/stm32f4xx_hal_msp.c
    ${PROJECT_SOURCE_DIR}/Core/Src/stm32f4xx_hal_timebase_tim.c
    ${PROJECT_SOURCE_DIR}/Core/Src/stm32f4xx_it.c
    ${PROJECT_SOURCE_DIR}/Core/Src/system_stm32f4xx.c

    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c

    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_3.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/timers.c

    ${PROJECT_SOURCE_DIR}/newlib/newlib.c

    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usbd_cdc_if.c
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usbd_desc.c
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App/usb_device.c
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/Target/usbd_conf.c
)

set(CPP_SOURCES
    ${PROJECT_SOURCE_DIR}/cpp/test.cpp
    ${PROJECT_SOURCE_DIR}/cpp/src/cpp-bootstrap.cpp
    ${PROJECT_SOURCE_DIR}/cpp/src/os/cpp-freertos-impl.cpp
)

set(ASM_SOURCES
    ${PROJECT_SOURCE_DIR}/startup_stm32f407xx.s
)

set(EXE_SOURCES
    ${ASM_SOURCES}
    ${C_SOURCES}
    ${CPP_SOURCES}
    ${H_HEADERS}
    ${HPP_HEADERS}
)

include_directories(
    ${PROJECT_SOURCE_DIR}/Core/Inc
    ${PROJECT_SOURCE_DIR}/cpp/include
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${PROJECT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${PROJECT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJECT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${PROJECT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/App
    ${PROJECT_SOURCE_DIR}/USB_DEVICE/Target
)

set(ELF_NAME ${PROJECT_NAME}.elf)
set(BIN_NAME ${PROJECT_NAME}.bin)
set(MAP_NAME ${PROJECT_NAME}.map)

set(LDSCRIPTS "-T STM32F407VGTx_FLASH.ld")
set(LDSCRIPTS_DIR "${PROJECT_SOURCE_DIR}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs -Xlinker --gc-sections -L${LDSCRIPTS_DIR} -Wl,-Map,\"${MAP_NAME}\" ${LDSCRIPTS} ")

add_executable(${ELF_NAME} ${EXE_SOURCES})

STM32_ADD_HEX_BIN_TARGETS(${ELF_NAME})
STM32_ADD_DUMP_TARGET(${ELF_NAME})
STM32_PRINT_SIZE_OF_TARGETS(${ELF_NAME})

