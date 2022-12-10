#ifndef SX1278_HPP
#define SX1278_HPP

#include "devices/sx1278/sx1278-driver-base.hpp"

#include <vector>
#include <memory>

class SX1278Device
{
public:
    enum class Power
    {
        POW_20DBM = 0,
        POW_17DBM = 1,
        POW_14DBM = 2,
        POW_11DBM = 3
    };


    enum class SpreadFactor
    {
        SF_6 = 0,
        SF_7 = 1,
        SF_8 = 2,
        SF_9 = 3,
        SF_10 = 4,
        SF_11 = 5,
        SF_12 = 6,
    };

    enum class Bandwidth
    {
        BW_7_8KHZ = 0,
        BW_10_4KHZ = 1,
        BW_15_6KHZ = 2,
        BW_20_8KHZ = 3,
        BW_31_2KHZ = 4,
        BW_41_7KHZ = 5,
        BW_62_5KHZ = 6,
        BW_125KHZ = 7,
        BW_250KHZ = 8,
        BW_500KHZ = 9
    };

    enum class CodingRate
    {
        CR_4_5 = 0,
        CR_4_6 = 1,
        CR_4_7 = 2,
        CR_4_8 = 3
    };

    enum class CRC_Mode
    {
        enabled = 0,
        disabled = 1
    };

    SX1278Device(std::shared_ptr<SX1278DriverBase> driver);

    /**
     * \brief Read byte from LoRa module
     *
     * Reads data from LoRa module from given address.
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  addr		Address from which data will be read
     *
     * \return              Read data
     */
    uint8_t SPIRead(uint8_t addr);

    /**
     * \brief Write byte to LoRa module
     *
     * Writes data to LoRa module under given address.
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  addr		Address under which data will be written
     * \param[in]  cmd 		Data to write
     */
    void SPIWrite(uint8_t addr, uint8_t cmd);

    /**
     * \brief Read data from LoRa module
     *
     * Reads data from LoRa module from given address.
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  addr		Address from which data will be read
     * \param[out] rxBuf    Pointer to store read data
     * \param[in]  length   Number of bytes to read
     */
    void SPIBurstRead(uint8_t addr, uint8_t *rxBuf,
            uint8_t length);

    /**
     * \brief Write data to LoRa module
     *
     * Writes data to LoRa module under given address.
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  addr		Address under which data will be written
     * \param[in]  txBuf    Pointer to data
     * \param[in]  length   Number of bytes to write
     */
    void SPIBurstWrite(uint8_t addr, uint8_t *txBuf,
            uint8_t length);

    /**
     * \brief Configure LoRa module
     *
     * Configure LoRa module according to parameters stored in
     * module structure.
     *
     * \param[in]  module	Pointer to LoRa structure
     */
    void config();

    /**
     * \brief Entry LoRa mode
     *
     * Module supports different operation mode.
     * To use LoRa operation mode one has to enter this
     * particular mode to transmit and receive data
     * using LoRa.
     *
     * \param[in]  module	Pointer to LoRa structure
     */
    void entryLoRa();

    /**
     * \brief Clear IRQ
     *
     * Clears interrupt flags.
     *
     * \param[in]  module	Pointer to LoRa structure
     */
    void clearLoRaIrq();

    /**
     * \brief Entry reception mode
     *
     * Entry reception mode
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  length   Length of message to be received
     * \param[in]  timeout  Timeout in [ms]
     *
     * \return     1 if entering reception mode
     *             0 if timeout was exceeded
     */
    int LoRaEntryRx(uint8_t length, uint32_t timeout);

    /**
     * \brief Read data
     *
     * Read data and store it in module's RX buffer
     *
     * \param[in]  module	Pointer to LoRa structure
     *
     * \return     returns number of read bytes
     */
    uint8_t LoRaRxPacket();

    /**
     * \brief Entry transmitter mode
     *
     * Entry transmitter mode
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  length   Length of message to be sent
     * \param[in]  timeout  Timeout in [ms]
     *
     * \return     1 if entering reception mode
     *             0 if timeout was exceeded
     */
    int LoRaEntryTx(uint8_t length, uint32_t timeout);

    /**
     * \brief Send data
     *
     * Transmit data
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  txBuf    Data buffer with data to be sent
     * \param[in]  length   Length of message to be sent
     * \param[in]  timeout  Timeout in [ms]
     *
     * \return     1 if entering reception mode
     *             0 if timeout was exceeded
     */
    int LoRaTxPacket(uint8_t *txBuf, uint8_t length, uint32_t timeout);

    /**
     * \brief Initialize LoRa module
     *
     * Initialize LoRa module and initialize LoRa structure.
     *
     * \param[in]  module	    Pointer to LoRa structure
     * \param[in]  frequency    Frequency in [Hz]
     * \param[in]  power        Power level, accepts SX1278_POWER_*
     * \param[in]  LoRa_SF      LoRa spread rate, accepts SX1278_LORA_SF_*
     * \param[in]  LoRa_BW      LoRa bandwidth, accepts SX1278_LORA_BW_*
     * \param[in]  LoRa_CR      LoRa coding rate, accepts SX1278_LORA_CR_*
     * \param[in]  LoRa_CRC_sum Hardware CRC check, SX1278_LORA_CRC_EN or
     *                          SX1278_LORA_CRC_DIS
     * \param[in]  packetLength Package length, no more than 256 bytes
     */
    void init(uint64_t frequency, SX1278Device::Power power,
              SX1278Device::SpreadFactor LoRa_SF, SX1278Device::Bandwidth LoRa_BW, SX1278Device::CodingRate LoRa_CR,
              SX1278Device::CRC_Mode LoRa_CRC_sum, uint8_t packetLength);

    /**
     * \brief Entry transmitter mode and send data
     *
     * Entry transmitter mode and send data.
     * Combination of LoRaEntryTx() and LoRaTxPacket().
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  txBuf    Data buffer with data to be sent
     * \param[in]  length   Length of message to be sent
     * \param[in]  timeout  Timeout in [ms]
     *
     * \return     1 if entered TX mode and sent data
     *             0 if timeout was exceeded
     */
    int transmit(uint8_t *txBuf, uint8_t length,
            uint32_t timeout);

    /**
     * \brief Entry reception mode
     *
     * Same as LoRaEntryRx()
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[in]  length   Length of message to be received
     * \param[in]  timeout  Timeout in [ms]
     *
     * \return     1 if entering reception mode
     *             0 if timeout was exceeded
     */
    int receive(uint8_t length, uint32_t timeout);

    /**
     * \brief Returns number of received data
     *
     * Returns the number of received data which are
     * held in internal buffer.
     * Same as LoRaRxPacket().
     *
     * \param[in]  module	Pointer to LoRa structure
     *
     * \return     returns number of read bytes
     */
    uint8_t available();

    /**
     * \brief Read received data to buffer
     *
     * Reads data from internal buffer to external
     * buffer. Reads exactly number of bytes which are stored in
     * internal buffer.
     *
     * \param[in]  module	Pointer to LoRa structure
     * \param[out] rxBuf    External buffer to store data.
     *                      External buffer is terminated with '\0'
     *                      character
     * \param[in]  length   Length of message to be received
     *
     * \return     returns number of read bytes
     */
    const std::vector<uint8_t>& get_rx_buffer() const;

    /**
     * \brief Returns RSSI (LoRa)
     *
     * Returns RSSI in LoRa mode.
     *
     * \param[in]  module	Pointer to LoRa structure
     *
     * \return     RSSI value
     */
    uint8_t RSSI_LoRa();

    /**
     * \brief Returns RSSI
     *
     * Returns RSSI (general mode).
     *
     * \param[in]  module	Pointer to LoRa structure
     *
     * \return     RSSI value
     */
    uint8_t RSSI();

    /**
     * \brief Enter standby mode
     *
     * Enters standby mode.
     *
     * \param[in]  module	Pointer to LoRa structure
     */
    void standby();

    /**
     * \brief Enter sleep mode
     *
     * Enters sleep mode.
     *
     * \param[in]  module	Pointer to LoRa structure
     */
    void sleep();

private:
    constexpr static int MAX_PACKET = 256;

    enum class SX1278_STATUS {
        SLEEP, STANDBY, TX, RX
    };

    constexpr static uint8_t power_values[4] = {
        0xFF, //20dbm
        0xFC, //17dbm
        0xF9, //14dbm
        0xF6, //11dbm
    };

    constexpr static uint8_t spread_factor_values[7] = { 6, 7, 8, 9, 10, 11, 12 };
    constexpr static uint8_t LoRa_bandwidth_values[10] = { 0, //   7.8KHz,
            1, //  10.4KHz,
            2, //  15.6KHz,
            3, //  20.8KHz,
            4, //  31.2KHz,
            5, //  41.7KHz,
            6, //  62.5KHz,
            7, // 125.0KHz,
            8, // 250.0KHz,
            9  // 500.0KHz
            };
    constexpr static uint8_t coding_rate_values[4] = { 0x01, 0x02, 0x03, 0x04 };
    constexpr static uint8_t CRC_sum_values[2] = { 0x01, 0x00 };

    std::shared_ptr<SX1278DriverBase> driver;

    uint64_t frequency;
    uint8_t power;
    uint8_t LoRa_SF;
    uint8_t LoRa_BW;
    uint8_t LoRa_CR;
    uint8_t LoRa_CRC_sum;
    uint8_t packetLength;

    SX1278_STATUS status;

    std::vector<uint8_t> m_rx_buffer;
    uint8_t readBytes;
};

#endif // SX1278_HPP
