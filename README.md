# WideUSB

Board based on STM32F4 to connect various perepherial over USB. May be useful with single-board computers (SoCs) like Raspberry Pi

Nearest roadmap
=====
- Networking
    - [ ] Retransmitting to another network devices
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

- Device core
	- [ ] All time-related -> std::chrono
    
- Modules interface
    - [x] For each module make 'module back' and 'module front'. Front should be avaliable from MCU too.
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

- [ ] DAC module

- [ ] ADC module

- [ ] GPIO module

- [ ] DAC module

- [ ] nfr24l01 device
	- [x] Driver abstraction
	- [x] Driver implementation
	- [x] Single device test
	- [ ] Double device test
	- [ ] Device class code refactoring

- [ ] LoRa module

- [ ] State saving in EEPROM

## Bugs
- [X] Monitor does not work with new front/back interface
