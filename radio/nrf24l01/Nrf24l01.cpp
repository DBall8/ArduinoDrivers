#include "Nrf24l01.hpp"
#include "drivers/timer/Delay.hpp"
#include "utilities/print/Print.hpp"

using namespace Spi;
using namespace Dio;

// Defined settings
const static uint8_t ADDRESS_LEN = 5u;
const static uint8_t MAX_TRANSMISSION_SIZE = 32u;
const static uint8_t DEFAULT_CHANNEL = 76;

// Registers
const static uint8_t REGISTER_MASK = 0x1F; // Maximum allowed register

const static uint8_t CONFIG_REG = 0x00; // Used for power on and interrupts
const static uint8_t EN_RXADDR = 0x02;  // Enable RX pipes
const static uint8_t SETUP_RETR = 0x04; // Retransmit settings
const static uint8_t RF_CH = 0x05;      // RF Channel
const static uint8_t RF_SETUP = 0x06;   // Radio settings
const static uint8_t STATUS = 0x07;     // Status register
const static uint8_t TX_ADDR = 0x10;    // Register to write transmit address to
const static uint8_t RX_ADDR_P0 = 0x0a; // Address for pipe 0
const static uint8_t RX_PW_P0 = 0x11;   // Payload size for pipe 0
const static uint8_t FIFO_STATUS = 0x17;// TX and RX empty and full status
const static uint8_t DYNPD = 0x1C;      // Enable dynamic payload lengths
const static uint8_t FEATURE = 0x1D;    // Enable Ack types

// Commands
const static uint8_t WRITE_MASK = 0x20;
const static uint8_t READ_MASK = 0x00;
const static uint8_t R_RX_PAYLOAD = 0x61;
const static uint8_t W_TX_PAYLOAD = 0xA0;
const static uint8_t FLUSH_TX = 0xE1;
const static uint8_t FLUSH_RX = 0xE2;
const static uint8_t NOP = 0xFF;

// Bits
// Config register
const static uint8_t MASK_RX_DR = 6;    // Mask read interrupt, 1 = disabled - 0 = enabled
const static uint8_t MASK_TX_DS = 5;    // Mask write interrupt, 1 = disabled - 0 = enabled
const static uint8_t MASK_MAX_RT = 4;   // Mask read buffer full interrupt, 1 = disabled - 0 = enabled
const static uint8_t EN_CRC = 3;        // Enable CRC
const static uint8_t CRC0 = 2;          // CRC encoding bytes, 0 = 1 byte - 1 = 2 bytes
const static uint8_t PWR_UP = 1;        // 1 = pwr up, 0 = pwr down
const static uint8_t PRIM_RX = 0;       // Rx/Tx mode, 1 = Rx, - 0 = Tx

// RF_SETUP
const static uint8_t RF_DR_LOW = 5;     // Used with RF_DR_HIGH to set bitrate
const static uint8_t RF_DR_HIGH = 3;    // [LOW, HIGH]
                                        // 00 = 1 Mb    ps, 01 = 2 Mbps, 10 = 250 kbps
const static uint8_t RF_PWR_MASK = 0x6; // Mask for RF PA power bits

// SETUP_RETR
const static uint8_t ARD = 4;
const static uint8_t ARC = 0;

// Status register bits
const uint8_t RX_DR = 0x06;
const uint8_t TX_DS = 0x05;
const uint8_t MAX_RT = 0x04;

// FIFO status register buts
const uint8_t TX_FULL = 0x05;
const uint8_t TX_EMPTY = 0x04;
const uint8_t RX_FULL = 0x01;
const uint8_t RX_EMPTY = 0x00;

// Reading pipes
const uint8_t NUM_RX_PIPES = 6;

const uint8_t RX_ADDRESSES[NUM_RX_PIPES] =
{
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F
};

const uint8_t RX_PW_WIDTHS[NUM_RX_PIPES] =
{
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16
};

enum ERX_BITS: uint8_t 
{
    ERX_0 = 0,
    ERX_1,
    ERX_2,
    ERX_3,
    ERX_4,
    ERX_5
};

namespace Radio
{
    Nrf24l01::Nrf24l01(IDio* pCePin,
                       ISpi* pSpi,
                       DataSpeed dataSpeed,
                       PaLevel paLevel,
                       uint32_t transferDelayMicroS):
        pCePin_(pCePin),
        pSpi_(pSpi),
        dataSpeed_(dataSpeed),
        paLevel_(paLevel),
        transferDelayMicroS_(transferDelayMicroS),
        payloadSize_(MAX_TRANSMISSION_SIZE),
        status_(RfStatus::IDLE),
        isInitialized_(false)
    {
    }

    Nrf24l01::~Nrf24l01()
    {

    }

    void Nrf24l01::enable()
    {
        pSpi_->enable();
    }

    void Nrf24l01::disable()
    {
        stop();
        pSpi_->disable();
    }

    void Nrf24l01::initialize()
    {
        enable();

        // Wait for settling time
        DELAY(150);

        // Set config
        uint8_t config = 0x00 | 
                        (1u << MASK_RX_DR) |    // Disable interrupts
                        (1u << MASK_TX_DS) |
                        (1u << MASK_MAX_RT) |
                        (1u << EN_CRC) |        // Enable CRC
                        (1u << CRC0);           // 2 bit CRC
        
        writeRegister(CONFIG_REG, config);

        // Set setup
        setPaLevel(paLevel_);
        setDataSpeed(dataSpeed_);
        setupRetries(15, 5);

        // Disable dynamic payload length
        writeRegister(DYNPD, 0);
        writeRegister(FEATURE, 0);

        clearStatusReg();

        setChannel(DEFAULT_CHANNEL);

        DELAY(2);
    }

    bool Nrf24l01::startTransmitting(uint8_t listenerId)
    {
        if (!isInitialized_) initialize();

        if (status_ == RfStatus::RECEIVING)
        {
            // We are receiving, stop doing so first
            stop();
        }

        char address[ADDRESS_LEN+1] = "00000";
        address[ADDRESS_LEN-1] = (char)listenerId;
        startTransmitting(address);

        status_ = RfStatus::TRANSMITTING;

        return true;
    }

    bool Nrf24l01::startReceiving(uint8_t listenerId)
    {
        if (!isInitialized_) initialize();

        if (status_ == RfStatus::TRANSMITTING)
        {
            // We are transmitting, stop so we can receive
            stop();
        }

        uint8_t pipeNum = listenerId % NUM_RX_PIPES;
        char address[ADDRESS_LEN+1] = "00000";
        address[ADDRESS_LEN-1] = (char)listenerId;
        startListening(pipeNum, address);

        status_ = RfStatus::RECEIVING;

        return true;
    }

    void Nrf24l01::setPaLevel(PaLevel paLevel)
    {
        uint8_t setup = readRegister(RF_SETUP);
        uint8_t setupBefore = setup;

        setup &= ~RF_PWR_MASK;
        setup |= paLevel;

        if (setup != setupBefore)
        {
            writeRegister(RF_SETUP, setup);
        }
    }

    void Nrf24l01::setDataSpeed(DataSpeed dataSpeed)
    {
        uint8_t setup = readRegister(RF_SETUP);
        uint8_t setupBefore = setup;

        switch(dataSpeed)
        {
            case RF_250_KBPS:
            {
                setup |= (1 << RF_DR_LOW);
                setup &= ~(1 << RF_DR_HIGH);
                break;
            }
            
            case RF_2_MBPS:
            {
                setup &= ~(1 << RF_DR_LOW);
                setup |= (1 << RF_DR_HIGH);
                break;
            }

            default:
            case RF_1_MBPS:
            {
                setup &= ~(1 << RF_DR_LOW);
                setup &= ~(1 << RF_DR_HIGH);
                break;
            }
        }

        // If register value has changed, update it
        if (setup != setupBefore)
        {
            writeRegister(RF_SETUP, setup);
        }
    }

    void Nrf24l01::setupRetries(uint8_t numRetries, uint8_t retransmitDelayMultiplier)
    {
        const static uint8_t MAX_RETRIES = 15u;
        const static uint16_t MAX_DELAY = 15u;
        
        if (numRetries > MAX_RETRIES) numRetries = MAX_RETRIES;
        if (retransmitDelayMultiplier > MAX_DELAY) retransmitDelayMultiplier = MAX_DELAY;

        uint8_t retryReg = 0x00 | (retransmitDelayMultiplier << ARD) | (numRetries << ARC);

        writeRegister(SETUP_RETR, retryReg);
    }

    void Nrf24l01::writeRegister(uint8_t reg, uint8_t value)
    {
        // Command is the register plus the write mask
        uint8_t command = WRITE_MASK | (reg & REGISTER_MASK);

        // Send the command followed by the value
        pSpi_->selectSlave();
        pSpi_->transfer(command, transferDelayMicroS_);
        pSpi_->transfer(value, transferDelayMicroS_);
        pSpi_->releaseSlave();
    }

    uint8_t Nrf24l01::readRegister(uint8_t reg)
    {
        // Command is the read mask plus the register
        uint8_t command = READ_MASK | (reg & REGISTER_MASK);

        // Send the register followed by any value, and get the respond
        // from the second transfer
        pSpi_->selectSlave();
        pSpi_->transfer(command, transferDelayMicroS_);
        uint8_t value = pSpi_->transfer(NOP, transferDelayMicroS_);
        pSpi_->releaseSlave();

        return value;
    }

    void Nrf24l01::writeRegister(uint8_t reg, uint8_t* buff, uint8_t numValues)
    {
        // Command is the register plus the write mask
        uint8_t command = WRITE_MASK | (reg & REGISTER_MASK);

        // Send the command followed by the value
        pSpi_->selectSlave();
        pSpi_->transfer(command, transferDelayMicroS_);
        pSpi_->write(buff, numValues, transferDelayMicroS_);
        pSpi_->releaseSlave();
    }

    void Nrf24l01::readRegister(uint8_t reg, uint8_t* buff, uint8_t numValues)
    {
        // Command is the read mask plus the register
        uint8_t command = READ_MASK | (reg & REGISTER_MASK);

        // Send the register followed by any value, and get the respond
        // from the second transfer
        pSpi_->selectSlave();
        pSpi_->transfer(command, transferDelayMicroS_);
        pSpi_->read(buff, numValues, transferDelayMicroS_);
        pSpi_->releaseSlave();
    }

    void Nrf24l01::sendCommand(uint8_t command)
    {
        pSpi_->selectSlave();
        pSpi_->transfer(command, transferDelayMicroS_);
        pSpi_->releaseSlave();
    }

    void Nrf24l01::setChannel(uint8_t channel)
    {
        const static uint8_t MAX_CHANNEL = 0x7f;
        writeRegister(RF_CH, channel & MAX_CHANNEL);
    }

    void Nrf24l01::flush()
    {
        sendCommand(FLUSH_TX);
        sendCommand(FLUSH_RX);
    }

    void Nrf24l01::powerUp(bool transmit)
    {
        // User existing config register, but add PWR_UP bit
        uint8_t config = readRegister(CONFIG_REG) |
                         (1 << PWR_UP);

        // Set PRIM_RX if receiving, clear it if transmitting
        if (transmit)
        {
            config &= ~(1 << PRIM_RX);
        }
        else
        {
            config |= (1 << PRIM_RX);
        }

        writeRegister(CONFIG_REG, config);

        // Give time for the radio to enter standby mode
        DELAY_MICROSECONDS(150);
    }

    void Nrf24l01::stop()
    {
        powerDown();
    }

    void Nrf24l01::powerDown()
    {
        uint8_t config = readRegister(CONFIG_REG) &
                         ~(1 << PWR_UP);

        writeRegister(CONFIG_REG, config);
    }

    void Nrf24l01::clearStatusReg()
    {
        writeRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
    }

    void Nrf24l01::startTransmitting(char* address)
    {
        // Set address for RX pipe 0 for getting ACKs
        writeRegister(RX_ADDR_P0, (uint8_t*)address, ADDRESS_LEN);

        // Set address to transmit on
        writeRegister(TX_ADDR, (uint8_t*)address, ADDRESS_LEN);

        // Write size for ACK
        writeRegister(RX_PW_P0, payloadSize_);

        flush();

        powerUp(true);
    }

    bool Nrf24l01::transmit(uint8_t* buff, uint8_t numBytes)
    {
        // Must have already called startTransmitting
        if (status_ != RfStatus::TRANSMITTING)
        {
            return false;
        }

        pSpi_->selectSlave();

        // Send TX command
        pSpi_->transfer(W_TX_PAYLOAD, transferDelayMicroS_);

        // Transmit data
        for (uint8_t i=0; i<numBytes; i++)
        {
            pSpi_->transfer(buff[i], transferDelayMicroS_);
        }

        // Send 0s to fill out rest of transmission size
        for (uint8_t i=numBytes; i<payloadSize_; i++)
        {
            pSpi_->transfer(0, transferDelayMicroS_);
        }
        pSpi_->releaseSlave();

        // Pulse CE high to start transmission
        pCePin_->set(L_HIGH);
        DELAY_MICROSECONDS(150);
        pCePin_->set(L_LOW);

        uint8_t completionMask = (1 << TX_DS) | (1 << MAX_RT);

        // Read status register until either Transmission complete
        // or Max retries flag have been set.
        uint8_t status;
        do
        {
            status = readRegister(STATUS);
        } while (!(status & completionMask));

        bool successfull = status & (1 << TX_DS);

#ifdef DEBUG_RADIO
        if (status & (1 << TX_DS))
        {
            PRINTLN("Transmission successful!");
        }
        else if (status & (1 << MAX_RT))
        {
            PRINTLN("Transmission failed: max retries used");
        }
        else
        {
            PRINTLN("Transmission failed: unknown reason");
        }
#endif

        if (!successfull) flush(); // Drop active transaction on failure

        clearStatusReg();

        return successfull;
    }

    bool Nrf24l01::startListening(uint8_t pipeIndex, char* address)
    {
        if (pipeIndex >= NUM_RX_PIPES) return false;

        writeRegister(RX_ADDRESSES[pipeIndex], (uint8_t*)address, ADDRESS_LEN);
        writeRegister(RX_PW_WIDTHS[pipeIndex], payloadSize_);

        writeRegister(EN_RXADDR, readRegister(EN_RXADDR) | (1 << pipeIndex));

        powerUp(false);

        flush();

         pCePin_->set(L_HIGH);

        DELAY_MICROSECONDS(150);

        return true;
    }

    bool Nrf24l01::isDataAvailable()
    {
        uint8_t fifoStatus = readRegister(FIFO_STATUS);
        uint8_t rxEmpty = fifoStatus & (1 << RX_EMPTY);

        return !rxEmpty;
    }

    bool Nrf24l01::receive(uint8_t* buff, uint8_t numBytes)
    { 
        // Must have already called startListening
        if (status_ != RfStatus::RECEIVING) return false;

        uint8_t data[payloadSize_];
        pSpi_->selectSlave();
        pSpi_->transfer(R_RX_PAYLOAD, transferDelayMicroS_);
        pSpi_->read(data, payloadSize_, transferDelayMicroS_);
        pSpi_->releaseSlave();

        for (uint8_t i=0; i<numBytes; i++)
        {
            buff[i] = data[i];
        }

        return true;
    }

    void Nrf24l01::stopListening()
    {
         pCePin_->set(L_LOW);
    }
}