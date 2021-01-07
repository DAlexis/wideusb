# WideUSB

Board based on STM32F4 to connect various perepherial over USB. May be useful with single-board computers (SoCs) like Raspberry Pi

Nearest roadmap
=====
0. [ ] Rework again network components
    - [x] Implement FragmentBuffer
    - [x] Implement sub-BufferAccessor 
    - [x] Channel binary layer
    - [x] Network binary layer
    - [ ] Transport binary layer
    
1. [ ] Running GPS functionality with hardcoded requests with python
    - [ ] Run new stack on board. Merge to master
2. [ ] Running this over C++ cli app with wideusb lib
3. [ ] Make python wrapper of C++ lib
4. [ ] Add streaming hardware functionality: DAC, ADC
5. [ ] Connect radio, nfr24 and LoRa
6. [ ] Setup forwarding
    - [ ] Implement binary serialization for forwarding 
6. [ ] Add state saving options
