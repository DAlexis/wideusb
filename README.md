# WideUSB

Board based on STM32F4 to connect various perepherial over USB. May be useful with single-board computers (SoCs) like Raspberry Pi

Nearest roadmap and tasks
=====
- Networking
    - [?] Retransmitting to another network devices
    - [ ] TimePlanner should return time of next activity
    - [ ] Internal transmission without network
    - [ ] Struct-based layers parameters
    - [ ] Send ack second time if already sended ?
    - [ ] Move base64 to communication
    - [x] Implement FragmentBuffer
    - [x] Implement sub-BufferAccessor 
    - [x] Channel binary layer
    - [x] Network binary layer
    - [x] Transport binary layer
    - [x] Socket state knowledge
    - [x] Socket queue limiting
    - [x] Forward sender address to client side
    - [x] Add callbacks when package received
    - [x] Add package inspection functions

- STM32 Hardware abstraction
    - [ ] Devices driver structure: Driver <- Hal driver <- instance with concrete handlers configured by Cube
	- [ ] Some kind of RTOS-based light asio analog
	
- Device core
	- [ ] All time-related -> std::chrono
	- [ ] Change debug printf to something without malloc https://stackoverflow.com/questions/262439/create-a-wrapper-function-for-malloc-and-free-in-c
    
- Modules interface
    - [x] For each module make 'module back' and 'module front'. Front should be avaliable from MCU too.
    - [ ] Change folders structure for modules
    - [ ] Derive backs from IModule

- Monitor module
    - [x] Run new stack on board
    - [x] Merge to master: remove old code and files

- GPS module
    - [ ] Fill thimestamp structs with content
        
- Asynchronous C++ API
    - [x] Add timeout for operations

- Synchronous python API
    - [ ] Replace condition variable with promise/future and wait_for (No! Timeout shoud be made in another way!)
    ...

- [x] DAC module

- [ ] ADC module

- [ ] GPIO module

- [ ] nfr24l01 device
	- [x] Driver abstraction
	- [x] Driver implementation
	- [x] Single device test
	- [x] Double device test
	- [ ] Device class code refactoring
		- [ ] SPI without IRQ

- [ ] LoRa module
	- [ ] Double device test
	- [ ] Code refactoring and adaptation

- [ ] State saving in EEPROM

- [ ] IR transmitter
- [ ] IR receiver

# Hardware
- [ ] Gen 1 PCB design

## Bugs
- [X] Monitor does not work with new front/back interface
- [ ] Wrong parameters value: file /home/dalexies/Projects/wideusb/source/stm32/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c on line 171 
      during HAL_NVIC_SetPriority in HAL_InitTick from HAL_InitTick (uwTickPrio); (stm32f4xx_hal_rcc.c:724) where uwTickPrio is set at 
      `uint32_t uwTickPrio   = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */` - this is generated by Cube
- [X] malloc(): mismatching next->prev_size (unsorted) on python exit


