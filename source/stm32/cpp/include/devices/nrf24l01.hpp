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

struct NRF24L01Config
{
    enum class CRCLen : uint8_t
    {
        crc_1_byte = 0,
        crc_2_byte = 1,
    };

    enum class CRCMode : uint8_t
    {
        disabled = 0,
        enabled = 1,
    };

    enum class AdressWidth
    {
        aw_3_bytes,
        aw_4_bytes,
        aw_5_bytes,
    };

    enum class Baudrate : uint8_t
    {
        br_1_mbit = 0,
        br_2_mbit = 1,
    };

    enum class TXPower : uint8_t
    {
        pw_m18db = 0,
        pw_m12db = 1,
        pw_m6db = 2,
        pw_0db = 3,
    };

    enum class LNAGain : uint8_t
    {
        disabled = 0,
        enabled = 1
    };

    uint8_t radio_channel = 1;
    AdressWidth address_width = AdressWidth::aw_5_bytes;
    Baudrate baudrate = Baudrate::br_1_mbit;
    TXPower tx_power = TXPower::pw_0db;
    LNAGain lna_gain = LNAGain::enabled;
    CRCMode crc_mode = CRCMode::enabled;
    CRCLen crc_len = CRCLen::crc_1_byte;
};

class NRF24L01Manager
{
public:
    constexpr static unsigned int payload_size = 32;
    constexpr static uint8_t defaultRadioChannel = 1;

    NRF24L01Manager(std::shared_ptr<NRF24L01IODriverBase> io_driver,
                    NRF24L01Config config);
    void set_data_received_callback(DataReceiveCallback callback);
    void set_tx_done_callback(TXDoneCallback callback);
    void set_tx_max_retries_callback(TXMaxRetriesCallback callback);

    void tick();

    bool is_tx_complete();
    void send(uint8_t size, uint8_t* data);
    void print_status();

    void set_rx_address(uint8_t channel, uint8_t* address);
    void set_tx_address(uint8_t* address);

    void enable_pipe(uint8_t pipe, bool enable);

    // CD register
    bool is_carrier_detected();

private:
    enum class InterruptionsMasks : uint8_t
    {
        max_retries = 1,
        tx_data_ready  = 2,
        rx_data_ready  = 4
    };

    enum class Power : uint8_t
    {
        POWER_OFF = 0,
        POWER_ON = 1
    };

    enum class RxTxMode : uint8_t
    {
        transmitter = 0,
        receiver = 1,
    };


    constexpr static uint32_t reinitPeriod = 10000000;
    constexpr static std::chrono::milliseconds time_enough_for_transmission{2};

    void flush_tx();
    void flush_rx();

    void switch_to_tx();
    void switch_to_rx();

    void receive_data(uint8_t size, uint8_t* data);

    void init_interrupts();
    void reg_write(uint8_t reg, uint8_t size, uint8_t *data);
    void reg_read(uint8_t reg, uint8_t size, uint8_t *data);

    // CONFIG register
    void set_config(InterruptionsMasks interruptionsMask,
            Power powerUP,
            RxTxMode isRecieving);

    // STATUS register parcers
    void update_status();
    bool is_rx_data_ready();
    bool is_tx_data_sent();
    bool is_max_retries_reached();
    int get_pipe_number_avaliable_for_rx_fifo();
    bool is_tx_fifo_full();

    void reset_interrupt_rx_data_ready();
    void reset_interrupt_tx_data_sent();
    void reset_interrupt_max_retries_reached();
    void reset_all_interrupts();


    // EN_AA register
    void set_auto_ACK(uint8_t channel, bool value);

    // SETUP_AW
    void set_adress_width(NRF24L01Config::AdressWidth width);

    // SETUP_RETR
    void setup_retransmission(uint8_t delay, uint8_t count);

    // RF_CH
    void set_RF_channel(uint8_t number);
    void clear_lost_packages_count();

    // RF_SETUP
    void set_RF_settings(NRF24L01Config::Baudrate use2MBits, NRF24L01Config::TXPower power, NRF24L01Config::LNAGain lna_gain);

    // OBSERVE_TX
    uint8_t get_lost_packages_count();
    uint8_t get_resent_packages_count();

    // RX_ADDR_Pn
    void read_rx_adresses();

    // TX_ADDR
    void read_tx_adress();

    // RX_PW_Pn
    void setRXPayloadLength(uint8_t channel, uint8_t payloadLength);

    // FIFO_STATUS
    bool is_reuse_enabled();
    bool is_tx_full();
    bool is_tx_empty();
    bool is_rx_full();
    bool is_rx_empty();

    // Wires
    void chip_enable_on();
    void chip_enable_off();
    void chip_select();
    void chip_deselect();
    void chip_enable_impulse();

    void on_rx_data_ready();
    void on_tx_data_sent();
    void on_max_retries_reached();

    std::shared_ptr<NRF24L01IODriverBase> m_io_driver;

    uint8_t m_reg_status = 0;
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

    uint64_t m_lastReinitTime = 0;

    NRF24L01Config m_config;
};


#endif // NRF24L01_HPP
