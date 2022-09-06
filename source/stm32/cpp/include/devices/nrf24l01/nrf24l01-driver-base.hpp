#ifndef NRF24L01DRIVERBASE_HPP
#define NRF24L01DRIVERBASE_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>

class NRF24L01IODriverBase
{
public:
    using EXTICallback = std::function<void(bool)>;
    virtual ~NRF24L01IODriverBase() = default;
    virtual void set_chip_select(bool state) = 0;
    virtual void set_chip_enable(bool state) = 0;
    virtual bool get_irq_pin() = 0;

    virtual uint8_t transmit_receive(uint8_t byte) = 0;
    virtual void transmit(uint8_t* data, size_t size) = 0;
    virtual void receive(uint8_t* data, size_t size) = 0;

    void transmit(uint8_t byte);

};

#endif // NRF24L01DRIVERBASE_HPP
