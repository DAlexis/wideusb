/*
*    Copyright (C) 2016 by Aleksey Bulatov
*
*    This file is part of Caustic Lasertag System project.
*
*    Caustic Lasertag System is free software:
*    you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Caustic Lasertag System is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Caustic Lasertag System.
*    If not, see <http://www.gnu.org/licenses/>.
*
*    @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
*/



#include "devices/nrf24l01.hpp"
#include "os/cpp-freertos.hpp"

#include <stdio.h>
#include <string.h>
/*
 * NRF24l01 Registers
 */

#define NRF_REG_CONFIG          0x00
#define NRF_REG_EN_AA           0x01
#define NRF_REG_EN_RXADDR       0x02
#define NRF_REG_SETUP_AW        0x03
#define NRF_REG_SETUP_RETR      0x04
#define NRF_REG_RF_CH           0x05
#define NRF_REG_RF_SETUP        0x06
#define NRF_REG_STATUS          0x07
#define NRF_REG_OBSERVE_TX      0x08
#define NRF_REG_CD              0x09
#define NRF_REG_RX_ADDR_P0      0x0A
#define NRF_REG_RX_ADDR_P1      0x0B
#define NRF_REG_RX_ADDR_P2      0x0C
#define NRF_REG_RX_ADDR_P3      0x0D
#define NRF_REG_RX_ADDR_P4      0x0E
#define NRF_REG_RX_ADDR_P5      0x0F
#define NRF_REG_TX_ADDR         0x10
#define NRF_REG_RX_PW_P0        0x11
#define NRF_REG_RX_PW_P1        0x12
#define NRF_REG_RX_PW_P2        0x13
#define NRF_REG_RX_PW_P3        0x14
#define NRF_REG_RX_PW_P4        0x15
#define NRF_REG_RX_PW_P5        0x16
#define NRF_REG_FIFO_STATUS     0x17
// N/A                          0x18
// N/A                          0x19
// N/A                          0x1A
// N/A                          0x1B
#define NRF_REG_DYNPD           0x1C
#define NRF_REG_FEATURE         0x1D


// Operations
#define NRF_NOP                 0xFF

/*
 * NRF2401 Register Fields
 */

// CONFIG
#define NRF_REGF_PRIM_RX            0
#define NRF_REGF_PWR_UP             1
#define NRF_REGF_CRCO               2
#define NRF_REGF_EN_CRC             3
#define NRF_REGF_MASK_MAX_RT        4
#define NRF_REGF_MASK_TX_DS         5
#define NRF_REGF_MASK_RX_DR         6

// EN_AA
#define NRF_REGF_ENAA_P0            0
#define NRF_REGF_ENAA_P1            1
#define NRF_REGF_ENAA_P2            2
#define NRF_REGF_ENAA_P3            3
#define NRF_REGF_ENAA_P4            4
#define NRF_REGF_ENAA_P5            5

// EN_RXADDR
#define NRF_REGF_ERX_P0             0
#define NRF_REGF_ERX_P1             1
#define NRF_REGF_ERX_P2             2
#define NRF_REGF_ERX_P3             3
#define NRF_REGF_ERX_P4             4
#define NRF_REGF_ERX_P5             5

// SETUP_AW
#define NRF_REGF_AW                 0

// SETUP_RETR
#define NRF_REGF_ARC                0
#define NRF_REGF_ARD                4

// RF_CH
#define NRF_REGF_RF_CH              0

// RF_SETUP
#define NRF_REGF_LNA_HCURR          0
#define NRF_REGF_RF_PWR             1
#define NRF_REGF_RF_DR              3
#define NRF_REGF_PLL_LOCK           4

// STATUS
#define NRF_REGF_TX_FULL            0
#define NRF_REGF_MAX_RT             4
#define NRF_REGF_TX_DS              5
#define NRF_REGF_RX_DR              6
#define NRF_REGF_RX_P_NO_MASK       0b00001110

// OBSERVE_TX
#define NRF_REGF_ARC_CNT_MASK            0b00001111
#define NRF_REGF_PLOS_CNT           4

// CD
#define NRF_REGF_CD                 0

// ADDR
#define NRF_REGF_ADDR_A             0
#define NRF_REGF_ADDR_B             1
#define NRF_REGF_ADDR_C             2
#define NRF_REGF_ADDR_D             3
#define NRF_REGF_ADDR_E             4

// RX_PW
#define NRF_REGF_PW                 0

// FIFO_STATUS
#define NRF_REGF_FIFO_RX_EMPTY      0
#define NRF_REGF_FIFO_RX_FULL       1
#define NRF_REGF_FIFO_TX_EMPTY      4
#define NRF_REGF_FIFO_TX_FULL       5
#define NRF_REGF_FIFO_TX_REUSE      6

// DYNPD
#define NRF_REGF_DPL_P0         0
#define NRF_REGF_DPL_P1         1
#define NRF_REGF_DPL_P2         2
#define NRF_REGF_DPL_P3         3
#define NRF_REGF_DPL_P4         4
#define NRF_REGF_DPL_P5         5

// FEATURE
#define NRF_REGF_EN_DYN_ACK     0
#define NRF_REGF_EN_ACK_PAY     1
#define NRF_REGF_EN_DPL         2

// Instructions
#define R_REGISTER(reg)         (0b00011111 & reg)
#define W_REGISTER(reg)         (0b00100000 | reg)
#define R_RX_PAYLOAD            0b01100001
#define W_TX_PAYLOAD            0b10100000
#define FLUSH_TX                0b11100001
#define FLUSH_RX                0b11100010
#define REUSE_TX_PL             0b11100011


NRF24L01Manager::NRF24L01Manager(std::shared_ptr<NRF24L01IODriverBase> io_driver, NRF24L01Config config) :
    m_io_driver(io_driver),
    m_config(config)
{
    chip_enable_off();
    chip_deselect();

    //////////////////////
    // Configuring nrf24l01
    // Global settings
    set_adress_width(m_config.address_width);
    set_RF_channel(m_config.radio_channel);
    //setupRetransmission(0, 0);

    // Reading addresses to local variables
    read_rx_adresses();
    read_tx_adress();

    // Setting up pipe 0
    set_auto_ACK(0, false);
    enable_pipe(0, true);
    setRXPayloadLength(0, payload_size);

    set_RF_settings(m_config.baudrate, m_config.tx_power, m_config.lna_gain);
    setup_retransmission(2, 15);
    switch_to_rx();

    if (m_TXAdress[0] == 0xe7)
    {
        printf("NRF24L01 initialization successfully done\n");
//        debug << "NRF24L01 initialization successfully done";
    } else {
        printf("Radio module seems to be not connected! Details:\r\n");
        print_status();
    }
}

void NRF24L01Manager::set_data_received_callback(DataReceiveCallback callback)
{
    m_RXcallback = callback;
}

bool NRF24L01Manager::is_tx_complete()
{
    return !m_waitingForTransmissionEnd;
}

void NRF24L01Manager::set_tx_max_retries_callback(TXMaxRetriesCallback callback)
{
    m_TXMaxRTcallback = callback;
}

void NRF24L01Manager::set_tx_done_callback(TXDoneCallback callback)
{
    m_TXDoneCallback = callback;
}

void NRF24L01Manager::switch_to_tx()
{
    chip_enable_off();
    set_config(InterruptionsMasks::max_retries, Power::POWER_ON, RxTxMode::transmitter);
    os::delay_iter_us(150);
}

void NRF24L01Manager::switch_to_rx()
{
    set_config(InterruptionsMasks::max_retries, Power::POWER_ON, RxTxMode::receiver);
    os::delay_iter_us(150);
    chip_enable_on();
    os::delay_iter_us(250);
}


void NRF24L01Manager::chip_enable_on()
{
    m_io_driver->set_chip_enable(true);
}

void NRF24L01Manager::chip_enable_off()
{
    m_io_driver->set_chip_enable(false);
}

void NRF24L01Manager::chip_select()
{
    m_io_driver->set_chip_select(false);
}

void NRF24L01Manager::chip_deselect()
{
    m_io_driver->set_chip_select(true);
}


void NRF24L01Manager::chip_enable_impulse()
{
    os::CriticalSection cs;

    chip_enable_on();
    os::delay_iter_us(15);
    chip_enable_off();
}

void NRF24L01Manager::reg_write(unsigned char reg, unsigned char size, unsigned char *data)
{
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(W_REGISTER(reg));
    if (size != 0)
    {
        m_io_driver->transmit(data, size);
    }
    chip_deselect();
}

void NRF24L01Manager::reg_read(unsigned char reg, unsigned char size, unsigned char *data)
{
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(R_REGISTER(reg));
    if (size != 0)
    {
        m_io_driver->receive(data, size);
    }
    chip_deselect();
}

void NRF24L01Manager::update_status()
{
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(NRF_NOP);
    chip_deselect();
}

/////////////////////
// STATUS
inline bool NRF24L01Manager::is_rx_data_ready()
{
    return m_reg_status & (1 << NRF_REGF_RX_DR);
}

inline bool NRF24L01Manager::is_tx_data_sent()
{
    return m_reg_status & (1 << NRF_REGF_TX_DS);
}

inline bool NRF24L01Manager::is_max_retries_reached()
{
    return m_reg_status & (1 << NRF_REGF_MAX_RT);
}

inline int NRF24L01Manager::get_pipe_number_avaliable_for_rx_fifo()
{
    return (m_reg_status & NRF_REGF_RX_P_NO_MASK) >> 1;
}

inline bool NRF24L01Manager::is_tx_fifo_full()
{
    return m_reg_status & (1 << NRF_REGF_TX_FULL);
}

void NRF24L01Manager::reset_interrupt_rx_data_ready()
{
    chip_select();
    m_io_driver->transmit(W_REGISTER(NRF_REG_STATUS));
    m_io_driver->transmit(1 << NRF_REGF_RX_DR);
    chip_deselect();
    update_status();
}

void NRF24L01Manager::reset_interrupt_tx_data_sent()
{
    chip_select();
    m_io_driver->transmit(W_REGISTER(NRF_REG_STATUS));
    m_io_driver->transmit(1 << NRF_REGF_TX_DS);
    chip_deselect();
    update_status();
}

void NRF24L01Manager::reset_interrupt_max_retries_reached()
{
    chip_select();
    m_io_driver->transmit(W_REGISTER(NRF_REG_STATUS));
    m_io_driver->transmit(1 << NRF_REGF_MAX_RT);
    chip_deselect();
    update_status();
}


/////////////////////
// CONFIG
void NRF24L01Manager::set_config(InterruptionsMasks interruptionsMask,
            Power powerUP,
            RxTxMode isRecieving)
{
    uint8_t reg_config =
            (uint8_t(interruptionsMask) << NRF_REGF_MASK_MAX_RT)
            | (uint8_t(m_config.crc_mode) << NRF_REGF_EN_CRC)
            | (uint8_t(m_config.crc_len) << NRF_REGF_CRCO)
            | (uint8_t(powerUP) << NRF_REGF_PWR_UP)
            | (uint8_t(isRecieving) << NRF_REGF_PRIM_RX);
    reg_write(NRF_REG_CONFIG, 1, &reg_config);
}

/////////////////////
// CD
bool NRF24L01Manager::is_carrier_detected()
{
    unsigned char result=0;
    reg_read(NRF_REG_CD, 1, &result);
    return result & (1 << NRF_REGF_CD);
}


/////////////////////
// EN_AA
void NRF24L01Manager::set_auto_ACK(unsigned char channel, bool value)
{
    unsigned char regValue=0;
    reg_read(NRF_REG_EN_AA, 1, &regValue);
    if (value)
    {
        regValue |= (1 << channel);
    } else {
        regValue &= ~(1 << channel);
    }
    reg_write(NRF_REG_EN_AA, 1, &regValue);
}

/////////////////////
// EN_RXADDR
void NRF24L01Manager::enable_pipe(unsigned char pipe, bool enable)
{
    unsigned char regValue=0;
    reg_read(NRF_REG_EN_RXADDR, 1, &regValue);
    if (enable)
        regValue |= (1 << pipe);
    else
        regValue &= ~(1 << pipe);
    reg_write(NRF_REG_EN_RXADDR, 1, &regValue);
}

/////////////////////
// SETUP_AW
void NRF24L01Manager::set_adress_width(NRF24L01Config::AdressWidth width)
{
    unsigned char regValue=0;
    switch(width)
    {
    case NRF24L01Config::AdressWidth::aw_3_bytes: regValue |= 0b00000001; break;
    case NRF24L01Config::AdressWidth::aw_4_bytes: regValue |= 0b00000010; break;
    case NRF24L01Config::AdressWidth::aw_5_bytes: regValue |= 0b00000011; break;
    }
    reg_write(NRF_REG_SETUP_AW, 1, &regValue);
}

/////////////////////
// SETUP_RETR
void NRF24L01Manager::setup_retransmission(unsigned char delay, unsigned char count)
{
    unsigned char regValue = (delay << NRF_REGF_ARD) | (count << NRF_REGF_ARC);
    reg_write(NRF_REG_SETUP_RETR, 1, &regValue);
}

/////////////////////
// RF_CH
void NRF24L01Manager::set_RF_channel(unsigned char number)
{
    uint8_t channel = number & 0b01111111;
    reg_write(NRF_REG_RF_CH, 1, &channel);
}

void NRF24L01Manager::clear_lost_packages_count()
{
    set_RF_channel(m_config.radio_channel);
}

/////////////////////
// RF_SETUP
void NRF24L01Manager::set_RF_settings(NRF24L01Config::Baudrate use2MBits, NRF24L01Config::TXPower power, NRF24L01Config::LNAGain lna_gain)
{
    unsigned char regValue =
            (uint8_t(use2MBits) << NRF_REGF_RF_DR)
            | (uint8_t(power) << NRF_REGF_RF_PWR)
            | (uint8_t(lna_gain) << NRF_REGF_LNA_HCURR);
    reg_write(NRF_REG_RF_SETUP, 1, &regValue);
}

/////////////////////
// OBSERVE_TX
unsigned char NRF24L01Manager::get_lost_packages_count()
{
    unsigned char regValue=0;
    reg_read(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue >> NRF_REGF_PLOS_CNT;
}

unsigned char NRF24L01Manager::get_resent_packages_count()
{
    unsigned char regValue=0;
    reg_read(NRF_REG_OBSERVE_TX, 1, &regValue);
    return regValue & NRF_REGF_ARC_CNT_MASK;
}

/////////////////////
// RX_ADDR_Pn
void NRF24L01Manager::set_rx_address(unsigned char channel, unsigned char* address)
{

    switch(channel)
    {
    default:
    case 0:
        memcpy(m_RXAdressP0, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
        reg_write(NRF_REG_RX_ADDR_P0, RADIO_ADDRESS_SIZE, address);
        break;
    case 1:
        memcpy(m_RXAdressP1, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
        reg_write(NRF_REG_RX_ADDR_P1, RADIO_ADDRESS_SIZE, address);
        break;
    case 2:
        m_RXAdressP2 = *address;
        reg_write(NRF_REG_RX_ADDR_P2, 1, address);
        break;
    case 3:
        m_RXAdressP3 = *address;
        reg_write(NRF_REG_RX_ADDR_P3, 1, address);
        break;
    case 4:
        m_RXAdressP4 = *address;
        reg_write(NRF_REG_RX_ADDR_P4, 1, address);
        break;
    case 5:
        m_RXAdressP5 = *address;
        reg_write(NRF_REG_RX_ADDR_P5, 1, address);
        break;
    }
}

void NRF24L01Manager::read_rx_adresses()
{
    reg_read(NRF_REG_RX_ADDR_P0, 5, m_RXAdressP0);
    reg_read(NRF_REG_RX_ADDR_P1, 5, m_RXAdressP1);
    reg_read(NRF_REG_RX_ADDR_P2, 1, &m_RXAdressP2);
    reg_read(NRF_REG_RX_ADDR_P3, 1, &m_RXAdressP3);
    reg_read(NRF_REG_RX_ADDR_P4, 1, &m_RXAdressP4);
    reg_read(NRF_REG_RX_ADDR_P5, 1, &m_RXAdressP5);
}

/////////////////////
// TX_ADDR
void NRF24L01Manager::set_tx_address(unsigned char* address)
{
    memcpy(m_TXAdress, address, RADIO_ADDRESS_SIZE*sizeof(unsigned char));
    reg_write(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

void NRF24L01Manager::read_tx_adress()
{
    reg_read(NRF_REG_TX_ADDR, RADIO_ADDRESS_SIZE, m_TXAdress);
}

/////////////////////
// RX_PW_Pn
void NRF24L01Manager::setRXPayloadLength(unsigned char channel, unsigned char payloadLength)
{
    payloadLength &= 0b00111111;
    //printf("Payload len: %u\n", payloadLength);
    switch(channel)
    {
    default:
    case 0: reg_write(NRF_REG_RX_PW_P0, 1, &payloadLength); break;
    case 1: reg_write(NRF_REG_RX_PW_P1, 1, &payloadLength); break;
    case 2: reg_write(NRF_REG_RX_PW_P2, 1, &payloadLength); break;
    case 3: reg_write(NRF_REG_RX_PW_P3, 1, &payloadLength); break;
    case 4: reg_write(NRF_REG_RX_PW_P4, 1, &payloadLength); break;
    case 5: reg_write(NRF_REG_RX_PW_P5, 1, &payloadLength); break;
    }
}

/////////////////////
// FIFO_STATUS
bool NRF24L01Manager::is_reuse_enabled()
{
    unsigned char regValue;
    reg_read(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_REUSE));
}

bool NRF24L01Manager::is_tx_full()
{
    unsigned char regValue;
    reg_read(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_FULL));
}

bool NRF24L01Manager::is_tx_empty()
{
    unsigned char regValue;
    reg_read(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_TX_EMPTY));
}

bool NRF24L01Manager::is_rx_full()
{
    unsigned char regValue;
    reg_read(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_FULL));
}

bool NRF24L01Manager::is_rx_empty()
{
    unsigned char regValue;
    reg_read(NRF_REG_FIFO_STATUS, 1, &regValue);
    return (regValue & (1 << NRF_REGF_FIFO_RX_EMPTY));
}

/////////////////////
// Send and receive
void NRF24L01Manager::send(unsigned char size, unsigned char* data)
{
    switch_to_tx();
    os::delay_iter_us(200);
//    systemClock->wait_us(200); // Strange workaround to prevent hard fault about here.
//                               // If you think that is bad - go and investigate just now.
//                               // It is something near hardware: big spi prescaler decrease fault prob.
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(W_TX_PAYLOAD);

    m_io_driver->transmit(data, size);
    chip_deselect();
    chip_enable_impulse();
    os::delay_iter_us(200);
    update_status();
    m_last_transmission_time = std::chrono::steady_clock::now();
    m_waitingForTransmissionEnd = true;
}

void NRF24L01Manager::receive_data(unsigned char size, unsigned char* data)
{
    chip_enable_off();
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(R_RX_PAYLOAD);
    m_io_driver->receive(data, size);
    chip_deselect();
}

void NRF24L01Manager::flush_tx()
{
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(FLUSH_TX);
    chip_deselect();
}

void NRF24L01Manager::flush_rx()
{
    chip_select();
    m_reg_status = m_io_driver->transmit_receive(FLUSH_RX);

    chip_deselect();
}

void NRF24L01Manager::print_status()
{
#ifndef DBG_NRF_DISABLE
    update_status();
    read_rx_adresses();
    read_tx_adress();
    printf("=== status: %x\n", m_reg_status);
    printf("RX Data ready: %d, TX Data sent: %d, Max retries: %d\n", (int) is_rx_data_ready(), (int) is_tx_data_sent(), (int) is_max_retries_reached());
    int pipe = get_pipe_number_avaliable_for_rx_fifo();
    if (pipe != 7)
        printf ("Pipe avaliable for RX FIFO: %d\n", pipe);
    else
        printf ("Pipe avaliable for RX FIFO: all empty\n");

    if (is_reuse_enabled()) printf ("Reuse avaliable enabled\n");
    if (is_tx_full()) printf("TX full\n");
    if (is_tx_empty()) printf("TX empty\n");
    if (is_rx_full()) printf("RX full\n");
    if (is_rx_empty()) printf ("RX empty\n");
    printf("Lost: %u, resent: %u\n", get_lost_packages_count(), get_resent_packages_count());
/*
    printf("Adresses:\n");
    printf("   TX: %x %x %x %x %x\n", m_TXAdress[0], m_TXAdress[1], m_TXAdress[2], m_TXAdress[3], m_TXAdress[4]);
    printf("RX P0: %x %x %x %x %x\n", m_RXAdressP0[0], m_RXAdressP0[1], m_RXAdressP0[2], m_RXAdressP0[3], m_RXAdressP0[4]);
    printf("RX P1: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP1[4]);
    printf("RX P2: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP2);
    printf("RX P3: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP3);
    printf("RX P4: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP4);
    printf("RX P5: %x %x %x %x %x\n", m_RXAdressP1[0], m_RXAdressP1[1], m_RXAdressP1[2], m_RXAdressP1[3], m_RXAdressP5);*/
#endif
}

void NRF24L01Manager::reset_all_interrupts()
{
    reset_interrupt_rx_data_ready();
    reset_interrupt_tx_data_sent();
    reset_interrupt_max_retries_reached();
}

void NRF24L01Manager::tick()
{
    // This is a workaround for strange behavior of some (all?) nrf24l01 modules:
    // sometimes module does not reset IRQ pin in case of TX data sent AND does not set
    // proper flag, so we simply check a timeout
    /*
    bool softwareDetectionOfTXDataSent = (
        m_waitingForTransmissionEnd
        && std::chrono::steady_clock::now() - m_last_transmission_time > time_enough_for_transmission
    );

//    if (!softwareDetectionOfTXDataSent && m_IRQPin->state() == true)
    if (!softwareDetectionOfTXDataSent && m_io_driver->get_irq_pin() == true)
    {
        reinitIfNeeded();
        return;
    }*/

    if (m_io_driver->get_irq_pin() == true)
        return;

    //printf("NRF IRQ detected\n");

    // Continuing here when IRQ detected
    update_status();
    print_status();
    if (is_rx_data_ready())
    {
        printf("RX data ready\n");
        on_rx_data_ready();
    }
    update_status();
    if (is_max_retries_reached())
    {
        printf("MAX RT\n");
        on_max_retries_reached();
    }
    update_status();
    if (is_tx_data_sent())
    {
        printf("Data sent\n");
        on_tx_data_sent();
    }

/*
    if (softwareDetectionOfTXDataSent)
    {
//        m_stager.stage("interrogate(): WA for TX freeze");
        onTXDataSent();
        return;
    }*/

//    resetRXDataReady();
    //if (!m_waitingForTransmissionEnd)
    //resetTXDataSent();
    //resetMaxRetriesReached();
}


void NRF24L01Manager::on_tx_data_sent()
{
//    m_stager.stage("onTXDataSent()");
    // Returning to default state: receiver
    reset_interrupt_tx_data_sent();
    switch_to_rx();
    m_waitingForTransmissionEnd = false;

    if (m_TXDoneCallback == nullptr) {
//        printf("TX done; no cb\n");
    } else
        m_TXDoneCallback();
}

void NRF24L01Manager::on_rx_data_ready()
{
    unsigned char pipe = get_pipe_number_avaliable_for_rx_fifo();
    unsigned char data[payload_size];
    while (!is_rx_empty())
    {
        receive_data(payload_size, data); // This updates m_status value
        if (m_RXcallback == nullptr) {
            printf("Warning: Callback is not set! RX data from pipe %d: \n", pipe);
        } else {
            m_RXcallback(pipe, data);
        }
    }
    chip_enable_on();
    reset_interrupt_rx_data_ready();
}

void NRF24L01Manager::on_max_retries_reached()
{
    if (m_TXMaxRTcallback == nullptr) {
        printf("Max RT; no cb\n");
    } else {
        m_TXMaxRTcallback();
    }
    // Clearing PLOS_CNT
    clear_lost_packages_count();

    reset_interrupt_max_retries_reached();
}
