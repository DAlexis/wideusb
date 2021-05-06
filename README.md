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
    
    
- Monitor module
    - [x] Run new stack on board
    - [x] Merge to master: remove old code and files

- GPS module
    - [ ] Fill thimestamp structs with content
        
- Asynchronous C++ API
    - [ ] Add timeout for operations

- Synchronous python API
    ...

- [ ] DAC module

- [ ] ADC module

- [ ] GPIO module

- [ ] DAC module

- [ ] nfr24l01 module

- [ ] LoRa module

- [ ] State saving in EEPROM
