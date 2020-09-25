#include "Atmega328I2c.hpp"

#include <avr/io.h>
#include "utilities/print/Print.hpp"
#include "drivers/timer/Delay.hpp"
#include "drivers/dio/atmega328/Atmega328Dio.hpp"

using namespace dio;
using namespace interrupt;
using namespace softwareTimer;

#define PRINT_I2C

namespace I2c
{
    // Mask for the status bits from the status register
    const static uint8_t STATUS_MASK = 0xf8;

    // Success codes for i2c actions
    const static uint8_t START_SENT = 0x08;
    const static uint8_t SLAVE_ADDRESS_SENT_TX = 0x18;
    const static uint8_t SLAVE_ADDRESS_SENT_RX = 0x40;
    const static uint8_t DATA_SENT = 0x28;
    const static uint8_t DATA_RECEIVED_AND_ACKED = 0x50;
    const static uint8_t DATA_RECEIVED_AND_NACKED = 0x58;

    Atmega328I2c::Atmega328I2c(I2cBitRate bitRate,
                            SoftwareTimer* pTimeoutTimer,
                            bool usePullup):
        pTimeoutTimer_(pTimeoutTimer),
        enabled_(false)
    {
        // Set bit rate
        setBitRate(bitRate);

        // If using internal pullups, configure i2c pins as inputs with internal pullups
        if (usePullup)
        {
            Atmega328Dio sda(Port::C, 4, Mode::INPUT, Level::L_LOW, false, true);
            Atmega328Dio scl(Port::C, 5, Mode::INPUT, Level::L_LOW, false, true);
        }
    }

    Atmega328I2c::~Atmega328I2c()
    {
    }

    void Atmega328I2c::enable()
    {
        // Set enable bit
        TWCR |= (1 << TWEN);
        enabled_ = true;
    }

    void Atmega328I2c::disable()
    {
        // Clear enable bit
        TWCR &= ~(1 << TWEN);
        enabled_ = false;
    }

    bool Atmega328I2c::transmit(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes)
    {
        if (!enabled_) return false;

        // Send the start message
        if (!start())
        {
            stop();
            return false;
        }
        
        // Send the slave ID plus write bit
        if (!sendSlaveAddress(slaveId, false))
        {
            stop();
            return false;
        }

        // Send data in the array
        for (uint16_t i=0; i<numBytes; i++)
        {
            if (!sendData(dataArray[i]))
            {
                stop();
                return false;
            }
        }

        // Stop communicaiton
        stop();

        return true;
    }

    bool Atmega328I2c::receive(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes)
    {
        if (!enabled_) return false;

        // Send start
        if (!start())
        {
            stop();
            return false;
        }

        // Send slave ID, with read bit
        if (!sendSlaveAddress(slaveId, true))
        {
            stop();
            return false;
        }

        // Receive each expected byte
        for (uint16_t i=0; i<numBytes; i++)
        {
            // Set isLast to true when receiving last byte for the array
            if (!receiveData(dataArray[i], (i == numBytes - 1)))
            {
                stop();
                return false;
            }
        }

        // Send stop and return success
        stop();
        return true;
    }

    bool Atmega328I2c::transmit(uint8_t slaveId, uint8_t data)
    {
        if (!enabled_) return false;

        // Send the start message
        if (!start())
        {
            stop();
            return false;
        }
        
        // Send the slave ID plus write bit
        if (!sendSlaveAddress(slaveId, false))
        {
            stop();
            return false;
        }

        // Send the data byte
        if (!sendData(data))
        {
            stop();
            return false;
        }

        // Stop communicaiton
        stop();

        return true;
    }

    bool Atmega328I2c::receive(uint8_t slaveId, uint8_t& data)
    {
        if (!enabled_) return false;

        // Send start
        if (!start())
        {
            stop();
            return false;
        }

        // Send slave ID, with read bit
        if (!sendSlaveAddress(slaveId, true))
        {
            stop();
            return false;
        }

        // Receive single byte, which is of course, the last byte
        if (!receiveData(data, true))
        {
            stop();
            return false;
        }

        // Send stop and return success
        stop();
        return true;
    }

    void Atmega328I2c::setBitRate(I2cBitRate bitRate)
    {
        switch (bitRate)
        {
            default:
            case I2cBitRate::BR_100_KBPS:
            {
                TWBR = 72; // TWBR = 72
                TWSR = 0;  // Prescaler = 1
                break;
            }

            case I2cBitRate::BR_400_KBPS:
            {
                TWBR = 12; // TWBR = 12
                TWSR = 0;  // Prescaler = 1
                break;
            }
        }
    }

    bool Atmega328I2c::start()
    {
        // Set initial conditions for the control register, with the start flag set
        TWCR |= (1 << TWEN) |  // Ensure the i2c is enabled
                (1 << TWSTA) | // Set the start flag
                (1 << TWINT);  // Set the interrupt flag to clear it, launching the start

        // Wait until sending the start completes
        if (!wait()) return false;
        
        uint8_t status = getStatus();

#ifdef PRINT_I2C
        if (status != START_SENT) PRINTLN("I2C Start failed, status = 0x%x", status);
#endif

        return (status == START_SENT);
    }

    void Atmega328I2c::stop()
    {
        TWCR &= ~(1 << TWSTA);  // Clear the start flag
        TWCR |= (1 << TWSTO) |  // Set the stop flag
                (1 << TWINT);   // Set the interrupt flag to clear it, launching the stop
    }

    void Atmega328I2c::reset()
    {
        // Clear control register
        TWCR = 0;
    }

    bool Atmega328I2c::sendSlaveAddress(uint8_t slaveId, bool read)
    {
        // Shift address left once to let the lowest bit be the r/w bit
        uint8_t slaveData = slaveId << 1;
        // Set read/write bit if this is a read, leave 0 if this is a write
        if (read) slaveData |= 0x01;

        // Transmit slave ID + r/w
        TWDR = slaveData;

        // Clear stop and start messages
        TWCR &= ~(1 << TWSTA) & ~(1 << TWSTO);
        TWCR |= (1 << TWINT);

        // Wait for transmission to complete, and check success
        if (!wait()) return false;
        uint8_t successCode = read ? SLAVE_ADDRESS_SENT_RX : SLAVE_ADDRESS_SENT_TX;
        uint8_t status = getStatus();

#ifdef PRINT_I2C
        if (status != successCode) PRINTLN("I2C Slave ID transmission failed, status = 0x%x", status);
#endif

        return (status == successCode);
    }

    bool Atmega328I2c::sendData(uint8_t data)
    {
        // Set data byte
        TWDR = data;

        // Reset interrupt flag to trigger transmission
        TWCR |= (1 << TWINT);

        // Wait for transmission to complete, and check success
        if (!wait()) return false;
        uint8_t status = getStatus();

#ifdef PRINT_I2C
        if (status != DATA_SENT) PRINTLN("I2C data transmission failed, status = 0x%x", status);
#endif
        return (status == DATA_SENT);
    }

    bool Atmega328I2c::receiveData(uint8_t& data, bool isLast)
    {
        if (isLast)
        {
            // This is the last byte to receive, so send a NACK after receiving it
            TWCR &= ~(1 << TWEA);
        }
        else
        {
            // Set flag to ACK the byte once its received, indicating more data is expected
            TWCR |= (1 << TWEA);
        }

        // Reset interrupt flag to continue transfer
        TWCR |= (1 << TWINT);
        if (!wait()) return false;

        // Check if transfer went as expected
        uint8_t successCode = isLast ? DATA_RECEIVED_AND_NACKED : DATA_RECEIVED_AND_ACKED;
        uint8_t status = getStatus();
        if (status != successCode)
        {
#ifdef PRINT_I2C
            PRINTLN("I2C data reception failed, status = 0x%x", status);
#endif
            return false;
        }

        // Read received byte
        data = TWDR;

        return true;
    }

    uint8_t Atmega328I2c::getStatus()
    {
        // Mask status bits, because this register is also used for the prescaler
        uint8_t status = TWSR;
        return status & STATUS_MASK;
    }

    bool Atmega328I2c::wait()
    {
        // Start timeout timer if one was given
        if (pTimeoutTimer_ != nullptr) pTimeoutTimer_->enable();

        // Busy loop until the TW interrupt flag is set
        while (!(TWCR & (1 << TWINT)))
        {
            // Check if timeout time has elapsed
            if ((pTimeoutTimer_ != nullptr) && 
                pTimeoutTimer_->hasOneShotPassed())
            {
#ifdef PRINT_I2C
                PRINTLN("I2c timeout timer elapsed.");
#endif
                pTimeoutTimer_->disable();
                return false;
            }
        }

        // Disable timer before continuing
        if (pTimeoutTimer_ != nullptr) pTimeoutTimer_->disable();
        return true;
    }
}