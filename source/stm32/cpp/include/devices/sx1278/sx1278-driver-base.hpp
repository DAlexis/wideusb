#ifndef SX1278DRIVERBASE_HPP
#define SX1278DRIVERBASE_HPP

#include <cstdint>

class SX1278DriverBase
{
public:
    virtual ~SX1278DriverBase() = default;

    /**
     * \brief Initialize hardware layer
     *
     * Clears NSS and resets LoRa module.
     *
     */
    virtual void init() = 0;

    /**
     * \brief Control NSS
     *
     * Clears and sets NSS according to passed value.
     *
     * \param[in]   value   1 sets NSS high, other value sets NSS low.
     */
    virtual void set_nss(int value) = 0;

    /**
     * \brief Resets LoRa module
     *
     * Resets LoRa module.
     */
    virtual void reset() = 0;

    /**
     * \brief Send command via SPI.
     *
     * Send single byte via SPI interface.
     *
     * \param[in]   cmd		Command
     */
    virtual void spi_command(uint8_t cmd) = 0;

    /**
     * \brief Reads data via SPI
     *
     * Reads data via SPI interface.
     *
     * \return				Read value
     */
    virtual uint8_t spi_read_byte() = 0;

    /**
     * \brief ms delay
     *
     * Milisecond delay.
     *
     * \param[in]   msec 		Number of milliseconds to wait
     */
    virtual void delay_ms(uint32_t msec) = 0;

    /**
     * \brief Reads DIO0 state
     *
     * Reads LoRa DIO0 state using GPIO.
     *
     * \return				0 if DIO0 low, 1 if DIO high
     */
    virtual int get_DIO0() = 0;

};

#endif // SX1278DRIVERBASE_HPP
