#include "devices/sx1278/sx1278.hpp"

#include "memory.h"

//RFM98 Internal registers Address
/********************LoRa mode***************************/
#define LR_RegFifo                                  0x00
// Common settings
#define LR_RegOpMode                                0x01
#define LR_RegFrMsb                                 0x06
#define LR_RegFrMid                                 0x07
#define LR_RegFrLsb                                 0x08
// Tx settings
#define LR_RegPaConfig                              0x09
#define LR_RegPaRamp                                0x0A
#define LR_RegOcp                                   0x0B
// Rx settings
#define LR_RegLna                                   0x0C
// LoRa registers
#define LR_RegFifoAddrPtr                           0x0D
#define LR_RegFifoTxBaseAddr                        0x0E
#define LR_RegFifoRxBaseAddr                        0x0F
#define LR_RegFifoRxCurrentaddr                     0x10
#define LR_RegIrqFlagsMask                          0x11
#define LR_RegIrqFlags                              0x12
#define LR_RegRxNbBytes                             0x13
#define LR_RegRxHeaderCntValueMsb                   0x14
#define LR_RegRxHeaderCntValueLsb                   0x15
#define LR_RegRxPacketCntValueMsb                   0x16
#define LR_RegRxPacketCntValueLsb                   0x17
#define LR_RegModemStat                             0x18
#define LR_RegPktSnrValue                           0x19
#define LR_RegPktRssiValue                          0x1A
#define LR_RegRssiValue                             0x1B
#define LR_RegHopChannel                            0x1C
#define LR_RegModemConfig1                          0x1D
#define LR_RegModemConfig2                          0x1E
#define LR_RegSymbTimeoutLsb                        0x1F
#define LR_RegPreambleMsb                           0x20
#define LR_RegPreambleLsb                           0x21
#define LR_RegPayloadLength                         0x22
#define LR_RegMaxPayloadLength                      0x23
#define LR_RegHopPeriod                             0x24
#define LR_RegFifoRxByteAddr                        0x25
#define LR_RegModemConfig3                          0x26
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B
#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64

/********************FSK/ook mode***************************/
#define  RegFIFO                0x00
#define  RegOpMode              0x01
#define  RegBitRateMsb      	0x02
#define  RegBitRateLsb      	0x03
#define  RegFdevMsb             0x04
#define  RegFdevLsb             0x05
#define  RegFreqMsb             0x06
#define  RegFreqMid             0x07
#define  RegFreqLsb         	0x08
#define  RegPaConfig            0x09
#define  RegPaRamp              0x0a
#define  RegOcp                 0x0b
#define  RegLna                 0x0c
#define  RegRxConfig            0x0d
#define  RegRssiConfig      	0x0e
#define  RegRssiCollision 		0x0f
#define  RegRssiThresh      	0x10
#define  RegRssiValue           0x11
#define  RegRxBw                0x12
#define  RegAfcBw               0x13
#define  RegOokPeak             0x14
#define  RegOokFix              0x15
#define  RegOokAvg              0x16
#define  RegAfcFei              0x1a
#define  RegAfcMsb              0x1b
#define  RegAfcLsb              0x1c
#define  RegFeiMsb              0x1d
#define  RegFeiLsb              0x1e
#define  RegPreambleDetect  	0x1f
#define  RegRxTimeout1      	0x20
#define  RegRxTimeout2      	0x21
#define  RegRxTimeout3      	0x22
#define  RegRxDelay             0x23
#define  RegOsc                 0x24
#define  RegPreambleMsb     	0x25
#define  RegPreambleLsb     	0x26
#define  RegSyncConfig      	0x27
#define  RegSyncValue1      	0x28
#define  RegSyncValue2      	0x29
#define  RegSyncValue3      	0x2a
#define  RegSyncValue4      	0x2b
#define  RegSyncValue5      	0x2c
#define  RegSyncValue6      	0x2d
#define  RegSyncValue7      	0x2e
#define  RegSyncValue8      	0x2f
#define  RegPacketConfig1       0x30
#define  RegPacketConfig2       0x31
#define  RegPayloadLength       0x32
#define  RegNodeAdrs            0x33
#define  RegBroadcastAdrs       0x34
#define  RegFifoThresh      	0x35
#define  RegSeqConfig1      	0x36
#define  RegSeqConfig2      	0x37
#define  RegTimerResol      	0x38
#define  RegTimer1Coef      	0x39
#define  RegSyncWord			0x39
#define  RegTimer2Coef      	0x3a
#define  RegImageCal            0x3b
#define  RegTemp                0x3c
#define  RegLowBat              0x3d
#define  RegIrqFlags1           0x3e
#define  RegIrqFlags2           0x3f
#define  RegDioMapping1			0x40
#define  RegDioMapping2			0x41
#define  RegVersion				0x42
#define  RegPllHop				0x44
#define  RegPaDac				0x4d
#define  RegBitRateFrac			0x5d

#define SX1278_LORA_SF_6		0
#define SX1278_LORA_SF_7		1
#define SX1278_LORA_SF_8		2
#define SX1278_LORA_SF_9		3
#define SX1278_LORA_SF_10		4
#define SX1278_LORA_SF_11		5
#define SX1278_LORA_SF_12		6

SX1278Device::SX1278Device(std::shared_ptr<SX1278DriverBase> driver) :
    driver(driver)
{
}

uint8_t SX1278Device::SPIRead(uint8_t addr)
{
    uint8_t tmp;
    driver->spi_command(addr);
    tmp = driver->spi_read_byte();
    driver->set_nss(1);
    return tmp;
}


void SX1278Device::SPIWrite(uint8_t addr, uint8_t cmd)
{
    driver->set_nss(0);
    driver->spi_command(addr | 0x80);
    driver->spi_command(cmd);
    driver->set_nss(1);
}

void SX1278Device::SPIBurstRead(uint8_t addr, uint8_t *rxBuf, uint8_t length)
{
    uint8_t i;
    if (length <= 1) {
        return;
    } else {
        driver->set_nss(0);
        driver->spi_command(addr);
        for (i = 0; i < length; i++) {
            *(rxBuf + i) = driver->spi_read_byte();
        }
        driver->set_nss(1);
    }
}


void SX1278Device::SPIBurstWrite(uint8_t addr, uint8_t *txBuf, uint8_t length)
{
    unsigned char i;
    if (length <= 1) {
        return;
    } else {
        driver->set_nss(0);
        driver->spi_command(addr | 0x80);
        for (i = 0; i < length; i++) {
            driver->spi_command(*(txBuf + i));
        }
        driver->set_nss(1);
    }
}

void SX1278Device::config()
{
    sleep(); //Change modem mode Must in Sleep mode
    driver->delay_ms(15);

    entryLoRa();
    //SPIWrite(module, 0x5904); //?? Change digital regulator form 1.6V to 1.47V: see errata note

    uint64_t freq = ((uint64_t) frequency << 19) / 32000000;
    uint8_t freq_reg[3];
    freq_reg[0] = (uint8_t) (freq >> 16);
    freq_reg[1] = (uint8_t) (freq >> 8);
    freq_reg[2] = (uint8_t) (freq >> 0);
    SPIBurstWrite(LR_RegFrMsb, (uint8_t*) freq_reg, 3); //setting  frequency parameter

    SPIWrite(RegSyncWord, 0x34);

    //setting base parameter
    SPIWrite(LR_RegPaConfig, power_values[power]); //Setting output power parameter

    SPIWrite(LR_RegOcp, 0x0B);			//RegOcp,Close Ocp
    SPIWrite(LR_RegLna, 0x23);		//RegLNA,High & LNA Enable
    if (spread_factor_values[LoRa_SF] == 6) {	//SFactor=6
        uint8_t tmp;
        SPIWrite(LR_RegModemConfig1,
                ((LoRa_bandwidth_values[LoRa_BW] << 4)
                        + (coding_rate_values[LoRa_CR] << 1) + 0x01)); //Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SPIWrite(LR_RegModemConfig2,
                ((spread_factor_values[LoRa_SF] << 4)
                        + (CRC_sum_values[LoRa_CRC_sum] << 2) + 0x03));

        tmp = SPIRead(0x31);
        tmp &= 0xF8;
        tmp |= 0x05;
        SPIWrite(0x31, tmp);
        SPIWrite(0x37, 0x0C);
    } else {
        SPIWrite(LR_RegModemConfig1,
                ((LoRa_bandwidth_values[LoRa_BW] << 4)
                        + (coding_rate_values[LoRa_CR] << 1) + 0x00)); //Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SPIWrite(LR_RegModemConfig2,
                ((spread_factor_values[LoRa_SF] << 4)
                        + (CRC_sum_values[LoRa_CRC_sum] << 2) + 0x00)); //SFactor &  LNA gain set by the internal AGC loop
    }

    SPIWrite(LR_RegModemConfig3, 0x04);
    SPIWrite(LR_RegSymbTimeoutLsb, 0x08); //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
    SPIWrite(LR_RegPreambleMsb, 0x00); //RegPreambleMsb
    SPIWrite(LR_RegPreambleLsb, 8); //RegPreambleLsb 8+4=12byte Preamble
    SPIWrite(REG_LR_DIOMAPPING2, 0x01); //RegDioMapping2 DIO5=00, DIO4=01
    readBytes = 0;
    standby(); //Entry standby mode
}

void SX1278Device::entryLoRa()
{
    SPIWrite(LR_RegOpMode, 0x88);
}

void SX1278Device::clearLoRaIrq()
{
    SPIWrite(LR_RegIrqFlags, 0xFF);
}

int SX1278Device::LoRaEntryRx(uint8_t length, uint32_t timeout)
{
    uint8_t addr;

    packetLength = length;

    config();		//Setting base parameter
    SPIWrite(REG_LR_PADAC, 0x84);	//Normal and RX
    SPIWrite(LR_RegHopPeriod, 0xFF);	//No FHSS
    SPIWrite(REG_LR_DIOMAPPING1, 0x01);//DIO=00,DIO1=00,DIO2=00, DIO3=01
    SPIWrite(LR_RegIrqFlagsMask, 0x3F);//Open RxDone interrupt & Timeout
    clearLoRaIrq();
    SPIWrite(LR_RegPayloadLength, length);//Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
    addr = SPIRead(LR_RegFifoRxBaseAddr); //Read RxBaseAddr
    SPIWrite(LR_RegFifoAddrPtr, addr); //RxBaseAddr->FiFoAddrPtr
    SPIWrite(LR_RegOpMode, 0x8d);	//Mode//Low Frequency Mode
    //SPIWrite(LR_RegOpMode,0x05);	//Continuous Rx Mode //High Frequency Mode
    readBytes = 0;

    while (1) {
        if ((SPIRead(LR_RegModemStat) & 0x04) == 0x04) {	//Rx-on going RegModemStat
            status = SX1278_STATUS::RX;
            return 1;
        }
        if (--timeout == 0) {
            driver->reset();
            config();
            return 0;
        }
        driver->delay_ms(1);
    }
}

uint8_t SX1278Device::LoRaRxPacket()
{
    unsigned char addr;
    unsigned char packet_size;

    if (driver->get_DIO0()) {
        memset(rxBuffer, 0x00, MAX_PACKET);

        addr = SPIRead(LR_RegFifoRxCurrentaddr); //last packet addr
        SPIWrite(LR_RegFifoAddrPtr, addr); //RxBaseAddr -> FiFoAddrPtr

        if (LoRa_SF == SX1278_LORA_SF_6) { //When SpreadFactor is six,will used Implicit Header mode(Excluding internal packet length)
            packet_size = packetLength;
        } else {
            packet_size = SPIRead(LR_RegRxNbBytes); //Number for received bytes
        }

        SPIBurstRead(0x00, rxBuffer, packet_size);
        readBytes = packet_size;
        clearLoRaIrq();
    }
    return readBytes;
}

int SX1278Device::LoRaEntryTx(uint8_t length, uint32_t timeout)
{
    uint8_t addr;
    uint8_t temp;

    packetLength = length;

    config(); //setting base parameter
    SPIWrite(REG_LR_PADAC, 0x87);	//Tx for 20dBm
    SPIWrite(LR_RegHopPeriod, 0x00); //RegHopPeriod NO FHSS
    SPIWrite(REG_LR_DIOMAPPING1, 0x41); //DIO0=01, DIO1=00,DIO2=00, DIO3=01
    clearLoRaIrq();
    SPIWrite(LR_RegIrqFlagsMask, 0xF7); //Open TxDone interrupt
    SPIWrite(LR_RegPayloadLength, length); //RegPayloadLength 21byte
    addr = SPIRead(LR_RegFifoTxBaseAddr); //RegFiFoTxBaseAddr
    SPIWrite(LR_RegFifoAddrPtr, addr); //RegFifoAddrPtr

    while (1) {
        temp = SPIRead(LR_RegPayloadLength);
        if (temp == length) {
            status = SX1278_STATUS::TX;
            return 1;
        }

        if (--timeout == 0) {
            driver->reset();
            config();
            return 0;
        }
    }
}

int SX1278Device::LoRaTxPacket(uint8_t *txBuffer, uint8_t length, uint32_t timeout)
{
    SPIBurstWrite(0x00, txBuffer, length);
    SPIWrite(LR_RegOpMode, 0x8b);	//Tx Mode
    while (1) {
        if (driver->get_DIO0()) { //if(Get_NIRQ()) //Packet send over
            SPIRead(LR_RegIrqFlags);
            clearLoRaIrq(); //Clear irq
            standby(); //Entry Standby mode
            return 1;
        }

        if (--timeout == 0) {
            driver->reset();
            config();
            return 0;
        }
        driver->delay_ms(1);
    }
}

void SX1278Device::init(uint64_t frequency, SX1278Device::Power power,
        SX1278Device::SpreadFactor LoRa_SF, SX1278Device::Bandwidth LoRa_BW, SX1278Device::CodingRate LoRa_CR,
        SX1278Device::CRC_Mode LoRa_CRC_sum, uint8_t packetLength)
{
    driver->init();
    this->frequency = frequency;
    this->power = int(power);
    this->LoRa_SF = int(LoRa_SF);
    this->LoRa_BW = int(LoRa_BW);
    this->LoRa_CR = int(LoRa_CR);
    this->LoRa_CRC_sum = int(LoRa_CRC_sum);
    this->packetLength = packetLength;
    config();
}

int SX1278Device::transmit(uint8_t *txBuf, uint8_t length, uint32_t timeout)
{
    if (LoRaEntryTx(length, timeout)) {
        return LoRaTxPacket(txBuf, length, timeout);
    }
    return 0;
}


int SX1278Device::receive(uint8_t length, uint32_t timeout)
{
    return LoRaEntryRx(length, timeout);
}

uint8_t SX1278Device::available()
{
    return LoRaRxPacket();
}

uint8_t SX1278Device::read(uint8_t *rxBuf, uint8_t length)
{
    if (length != readBytes)
        length = readBytes;
    memcpy(rxBuf, rxBuffer, length);
    rxBuf[length] = '\0';
    readBytes = 0;
    return length;
}

uint8_t SX1278Device::RSSI_LoRa()
{
    uint32_t temp = 10;
    temp = SPIRead(LR_RegRssiValue); //Read RegRssiValue, Rssi value
    temp = temp + 127 - 137; //127:Max RSSI, 137:RSSI offset
    return (uint8_t) temp;
}

uint8_t SX1278Device::RSSI()
{
    uint8_t temp = 0xff;
    temp = SPIRead(RegRssiValue);
    temp = 127 - (temp >> 1);	//127:Max RSSI
    return temp;
}

void SX1278Device::standby()
{
    SPIWrite(LR_RegOpMode, 0x09);
    status = SX1278_STATUS::STANDBY;
}

void SX1278Device::sleep()
{
    SPIWrite(LR_RegOpMode, 0x08);
    status = SX1278_STATUS::SLEEP;
}
