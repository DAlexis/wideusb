#ifndef NRF24L01_HPP
#define NRF24L01_HPP

#include "os/os-types.hpp"
#include "devices/nrf24l01-driver-base.hpp"

#include <functional>
#include <cstdint>
#include <memory>
#include <chrono>

#define RADIO_ADDRESS_SIZE  5

using DataReceiveCallback = std::function<void(uint8_t/* channel*/, uint8_t*/* data*/)>;
using TXMaxRetriesCallback = std::function<void(void)>;
using TXDoneCallback = std::function<void(void)>;

class NRF24L01Manager
{
public:
    enum EnableDisable
    {
        DISABLE_OPTION = 0,
        ENABLE_OPTION = 1,
    };

    constexpr static unsigned int payloadSize = 32;
    constexpr static uint8_t defaultRadioChannel = 1;

    NRF24L01Manager(std::shared_ptr<NRF24L01IODriverBase> io_driver,
                    uint8_t radioChannel = defaultRadioChannel);

    uint32_t getPayloadSize();

    void setDataReceiveCallback(DataReceiveCallback callback);
    void setTXDoneCallback(TXDoneCallback callback);
    void sendData(uint8_t size, uint8_t* data);
    void setRXAddress(uint8_t channel, uint8_t* address);
    void setTXAddress(uint8_t* address);
    void printStatus();


    void setTXMaxRetriesCallback(TXMaxRetriesCallback callback);
    void resetAllIRQ();

    void interrogate();

    // EN_RXARRD
    void enablePipe(uint8_t pipe, uint8_t value);

    void flushTX();
    void flushRX();

    void enableDebug(bool debug = true);

private:
    constexpr static uint32_t reinitPeriod = 10000000;

    enum Power
    {
        POWER_OFF = 0,
        POWER_ON = 1
    };

    enum InterruptionsMasks
    {
        IM_MASK_MAX_RETRIES = 1,
        IM_MASK_TX_DATA_READY  = 2,
        IM_MASK_RX_DATA_READY  = 4
    };

    enum CRCEnableDisable
    {
        CRC_DISABLE = 0,
        CRC_ENABLE = 1
    };

    enum CRCLen
    {
        CRC1BYTE = 0,
        CRC2BYTES = 1,
    };

    enum AdressWidth
    {
        AW_3BYTES,
        AW_4BYTES,
        AW_5BYTES,
    };

    enum Baudrate
    {
        BR_1MBIT = 0,
        BR_2MBIT = 1,
    };

    enum TXPower
    {
        PW_m18DB = 0,
        PW_m12DB = 1,
        PW_m6DB = 2,
        PW_0DB = 3,
    };

    enum LNAGain
    {
        LNA_DISABLE = 0,
        LNA_ENABLE = 1,
    };

    enum RXTXMODE
    {
        MODE_TRANSMITTER = 0,
        MODE_RECEIVER = 1,
    };

//    constexpr static uint16_t timeEnoughForTransmission = 1000;
    constexpr static std::chrono::milliseconds time_enough_for_transmission{2};

    void switchToTX();
    void switchToRX();

    void receiveData(uint8_t size, uint8_t* data);

    void initInterrupts();
    void writeReg(uint8_t reg, uint8_t size, uint8_t *data);
    void readReg(uint8_t reg, uint8_t size, uint8_t *data);

    void extiHandler(bool state);

    // CONFIG register
    void setConfig(uint8_t interruptionsMask,
            uint8_t enableCRC,
            uint8_t CRC2bytes,
            uint8_t powerUP,
            uint8_t isRecieving);

    void switchToReceiver();
    void switchToTranmitter();

    // STATUS register parcers
    void updateStatus();
    inline bool isRXDataReady();
    inline bool isTXDataSent();
    inline bool isMaxRetriesReached();
    inline int getPipeNumberAvaliableForRXFIFO();
    inline bool isTXFIFOFull();

    void resetRXDataReady();
    void resetTXDataSent();
    void resetMaxRetriesReached();

    // CD register
    bool isCarrierDetected();

    // EN_AA register
    void setAutoACK(uint8_t channel, bool value);

    // SETUP_AW
    void setAdressWidth(AdressWidth width);

    // SETUP_RETR
    void setupRetransmission(uint8_t delay, uint8_t count);

    // RF_CH
    void setRFChannel(uint8_t number);
    void clearLostPackagesCount();

    // RF_SETUP
    void setRFSettings(uint8_t use2MBits, uint8_t power, uint8_t LNAGain);

    // OBSERVE_TX
    uint8_t getLostPackagesCount();
    uint8_t getResentPackagesCount();

    // RX_ADDR_Pn
    void readRXAdresses();

    // TX_ADDR
    void readTXAdress();

    // RX_PW_Pn
    void setRXPayloadLength(uint8_t channel, uint8_t payloadLength);

    // FIFO_STATUS
    bool isReuseEnabled();
    bool isTXFull();
    bool isTXEmpty();
    bool isRXFull();
    bool isRXEmpty();

    // Wires
    void chipEnableOn();
    void chipEnableOff();
    void chipSelect();
    void chipDeselect();
    void CEImpulse();

    void onRXDataReady();
    void onTXDataSent();
    void onMaxRetriesReached();

    void reinitIfNeeded();

    std::shared_ptr<NRF24L01IODriverBase> m_io_driver;

    uint8_t m_SPIIndex = 0;
    uint8_t m_radioChannel = 1;

    bool m_needInterrogation = false;

    uint8_t m_status = 0;
    uint8_t m_config = 0;
    uint8_t m_RFChannel = defaultRadioChannel;
    uint8_t m_TXAdress[RADIO_ADDRESS_SIZE];
    uint8_t m_RXAdressP0[RADIO_ADDRESS_SIZE];
    uint8_t m_RXAdressP1[RADIO_ADDRESS_SIZE];
    uint8_t m_RXAdressP2;
    uint8_t m_RXAdressP3;
    uint8_t m_RXAdressP4;
    uint8_t m_RXAdressP5;

    DataReceiveCallback m_RXcallback = nullptr;
    TXMaxRetriesCallback m_TXMaxRTcallback = nullptr;
    TXDoneCallback m_TXDoneCallback = nullptr;

    std::chrono::steady_clock::time_point m_last_transmission_time;
//    uint64_t m_lastTransmissionTime = 0;
    bool m_waitingForTransmissionEnd = false;

    bool m_debug = false;

    uint64_t m_lastReinitTime = 0;
};


#endif // NRF24L01_HPP
