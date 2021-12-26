#include "devices/nrf24l01-driver-base.hpp"

void NRF24L01IODriverBase::transmit(uint8_t byte)
{
    transmit(&byte, 1);
}
