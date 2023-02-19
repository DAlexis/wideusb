cmake_minimum_required(VERSION 3.8)

project(wideusb C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

IF(NOT ARM_NONE_EABI_GCC_TOOLCHAIN)
    message(FATAL_ERROR "Toolchain arm-none-eabi not selected! Please run cmake with -DCMAKE_TOOLCHAIN_FILE=stm32f407.cmake")
ENDIF()

set(H_HEADERS
    ./USB_DEVICE/Target/usbd_conf.h
    ./USB_DEVICE/App/usb_device.h
    ./USB_DEVICE/App/usbd_cdc_if.h
    ./USB_DEVICE/App/usbd_desc.h
    ./newlib/newlib-monitor.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_uart.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pcd_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ramfunc.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_exti.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dac_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dma_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_dac.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_cortex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_def.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_gpio.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_usb.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_flash_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_rcc_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_pwr_ex.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h
    ./Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_spi.h
    ./Drivers/CMSIS/Include/core_cm0plus.h
    ./Drivers/CMSIS/Include/cmsis_version.h
    ./Drivers/CMSIS/Include/core_armv8mbl.h
    ./Drivers/CMSIS/Include/core_sc300.h
    ./Drivers/CMSIS/Include/cmsis_armclang.h
    ./Drivers/CMSIS/Include/cmsis_armcc.h
    ./Drivers/CMSIS/Include/core_cm1.h
    ./Drivers/CMSIS/Include/core_cm7.h
    ./Drivers/CMSIS/Include/cmsis_compiler.h
    ./Drivers/CMSIS/Include/tz_context.h
    ./Drivers/CMSIS/Include/core_cm23.h
    ./Drivers/CMSIS/Include/cmsis_iccarm.h
    ./Drivers/CMSIS/Include/mpu_armv8.h
    ./Drivers/CMSIS/Include/core_armv8mml.h
    ./Drivers/CMSIS/Include/core_cm33.h
    ./Drivers/CMSIS/Include/core_sc000.h
    ./Drivers/CMSIS/Include/core_cm0.h
    ./Drivers/CMSIS/Include/core_cm3.h
    ./Drivers/CMSIS/Include/cmsis_gcc.h
    ./Drivers/CMSIS/Include/core_cm4.h
    ./Drivers/CMSIS/Include/mpu_armv7.h
    ./Drivers/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h
    ./Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h
    ./Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h
    ./cpp/include/cpp-bootstrap.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/FreeRTOS.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/event_groups.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/projdefs.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/atomic.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/deprecated_definitions.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/mpu_wrappers.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/queue.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/semphr.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/task.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/StackMacros.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/croutine.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/timers.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/mpu_prototypes.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/stack_macros.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/portable.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/stream_buffer.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/list.h
    ./Middlewares/Third_Party/FreeRTOS/Source/include/message_buffer.h
    ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.h
    ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/portmacro.h
    ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/usbd_cdc.h
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_def.h
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ctlreq.h
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_core.h
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ioreq.h
    ./Core/Inc/stm32f4xx_it.h
    ./Core/Inc/main.h
    ./Core/Inc/stm32f4xx_hal_conf.h
    ./Core/Inc/dac.h
    ./Core/Inc/FreeRTOSConfig.h
    ./Core/Inc/tim.h
    ./Core/Inc/gpio.h
    ./Core/Inc/usart.h
    ./Core/Inc/dma.h
    ./Core/Inc/spi.h
#    stm32_lock.h

    ./cpp/include/host-communication/usb-physical-layer.hpp
    ./cpp/include/host-communication/usbd-input-handle.h
    ./cpp/include/modules/gps-impl.hpp
    ./cpp/include/modules/dac-impl.hpp
    ./cpp/include/modules/gps/nmea-receiver.hpp
    ./cpp/include/modules/gps/precision-timer.hpp
    ./cpp/include/modules/monitor-impl.hpp
    ./cpp/include/os/cpp-freertos.hpp
    ./cpp/include/os/os-types.hpp
    ./cpp/include/os/async-worker.hpp
    ./cpp/include/data-saver.hpp
    ./cpp/include/macro.hpp
    ./cpp/include/wideusb-device.hpp
    ./cpp/include/net-srv-runner.hpp
    ./cpp/include/socket-queue-rtos.hpp
    ./cpp/include/devices/nrf24l01/nrf24l01.hpp
    ./cpp/include/devices/nrf24l01/nrf24l01-driver-base.hpp
    ./cpp/include/devices/nrf24l01/nrf24l01-driver-hal.hpp

    ./cpp/include/devices/hal.hpp
    ./cpp/include/devices/sx1278/sx1278.hpp
    ./cpp/include/devices/sx1278/SX1278.h
    ./cpp/include/devices/sx1278/SX1278_hw.h
    ./cpp/include/devices/sx1278/sx1278-driver-base.hpp
    ./cpp/include/devices/sx1278/sx1278-driver-hal.hpp
    ./cpp/include/devices/sx1278/sx1278-physical-layer.hpp
)


set(C_SOURCES
    ./USB_DEVICE/Target/usbd_conf.c
    ./USB_DEVICE/App/usbd_cdc_if.c
    ./USB_DEVICE/App/usbd_desc.c
    ./USB_DEVICE/App/usb_device.c
    ./newlib/newlib.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ./Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    ./Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    ./Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ./Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ./Middlewares/Third_Party/FreeRTOS/Source/list.c
    ./Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_3.c
    ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ./Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ./Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ./Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ./Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ./Core/Src/dac.c
    ./Core/Src/stm32f4xx_it.c
    ./Core/Src/stm32f4xx_hal_msp.c
    ./Core/Src/dma.c
    ./Core/Src/system_stm32f4xx.c
    ./Core/Src/gpio.c
    ./Core/Src/tim.c
    ./Core/Src/stm32f4xx_hal_timebase_tim.c
    ./Core/Src/main.c
    ./Core/Src/usart.c
    ./Core/Src/freertos.c
    ./Core/Src/spi.c
#    newlib_lock_glue.c
)

set(CPP_SOURCES
    ./cpp/src/host-communication/usb-physical-layer.cpp
    ./cpp/src/data-saver.cpp
    ./cpp/src/net-srv-runner.cpp
    ./cpp/src/modules/gps/nmea-receiver.cpp
    ./cpp/src/modules/gps/precision-timer.cpp
    ./cpp/src/modules/monitor-impl.cpp
    ./cpp/src/modules/dac-impl.cpp
    ./cpp/src/modules/gps-impl.cpp
    ./cpp/src/os/cpp-freertos-impl.cpp
    ./cpp/src/os/safe-malloc-free.cpp
    ./cpp/src/os/async-worker.cpp
    ./cpp/src/cpp-bootstrap.cpp
    ./cpp/src/wideusb-device.cpp
    ./cpp/src/socket-queue-rtos.cpp
    ./cpp/src/devices/nrf24l01/nrf24l01.cpp
    ./cpp/src/devices/nrf24l01/nrf24l01-driver-base.cpp
    ./cpp/src/devices/nrf24l01/nrf24l01-driver-hal.cpp

    ./cpp/src/devices/sx1278/sx1278.cpp
    ./cpp/src/devices/sx1278/SX1278.c
    ./cpp/src/devices/sx1278/SX1278_hw.c
    ./cpp/src/devices/sx1278/sx1278-driver-hal.cpp
    ./cpp/src/devices/sx1278/sx1278-driver-base.cpp
    ./cpp/src/devices/sx1278/sx1278-physical-layer.cpp
)

set(ASM_SOURCES
    startup_stm32f407xx.s
)

set(EXE_SOURCES
    ${ASM_SOURCES}
    ${C_SOURCES}
    ${CPP_SOURCES}
    ${H_HEADERS}
    ${HPP_HEADERS}
)

set(ELF_NAME ${PROJECT_NAME}.elf)
set(BIN_NAME ${ELF_NAME}.bin)
set(MAP_NAME ${PROJECT_NAME}.map)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=incompatible-pointer-types")

add_executable(${ELF_NAME} ${EXE_SOURCES})

target_compile_definitions(${ELF_NAME} PUBLIC STM32_THREAD_SAFE_STRATEGY=4)

target_link_libraries(${ELF_NAME}
    wideusb-common
    -Wl,-Map,\"${MAP_NAME}\"
    -Wl,--wrap=malloc
    -Wl,--wrap=free
    -L${PROJECT_SOURCE_DIR}
    "-T STM32F407VGTx_FLASH.ld")

target_include_directories(${ELF_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/newlib
    ${PROJECT_SOURCE_DIR}/Core/Inc
    ${PROJECT_SOURCE_DIR}/cpp/include
    ${PROJECT_SOURCE_DIR}/c-tools/include
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
    ${PROJECT_SOURCE_DIR}/../../communication/include
)

configure_file(flash.sh.in flash.sh)

STM32_ADD_HEX_BIN_TARGETS(${ELF_NAME})
#STM32_ADD_DUMP_TARGET(${ELF_NAME})
STM32_PRINT_SIZE_OF_TARGETS(${ELF_NAME})

