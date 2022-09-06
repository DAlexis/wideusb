#ifndef DEVICES_HAL_HPP
#define DEVICES_HAL_HPP

#include "stm32f4xx_hal.h"

struct GPIOPin
{
    GPIOPin(GPIO_TypeDef *gpio, uint16_t pin) :
        gpio(gpio), pin(pin)
    { }

    GPIO_TypeDef *gpio = nullptr;
    uint16_t pin = 0;
};


#endif // DEVICES_HAL_HPP
