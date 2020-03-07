#include "Atmega328Spi.hpp"
#include <Arduino.h>

namespace spi
{
    void Atmega328Spi::SendByte(uint8_t data){ SPDR = data; }
    uint8_t Atmega328Spi::GetByte(){ return SPDR; }

    Atmega328Spi::Atmega328Spi(uint8_t mosi,
                               uint8_t miso,
                               uint8_t clock,
                               uint8_t slaveSelect,
                               bool isMaster,
                               SpiClock clockSpeed,
                               SpiPolarity polarity,
                               SpiPhase phase)
    {
        slaveSelect_ = slaveSelect;
        isMaster_ = isMaster;

        if (isMaster_)
        {
            pinMode(miso, INPUT);
            pinMode(mosi, OUTPUT);
            pinMode(clock, OUTPUT);
            pinMode(SS, OUTPUT);
            pinMode(slaveSelect_, OUTPUT);
            releaseSlave();

            setSpiClockSpeed(clockSpeed);

            SPCR = 0 | (1 << MSTR) |    // Enable master mode
                (polarity << CPOL) | // Set polarity
                (phase << CPHA);     // Set phase
            }
        else
        {
            pinMode(miso, OUTPUT);
            pinMode(mosi, INPUT);
            pinMode(clock, INPUT);
            pinMode(slaveSelect, INPUT);
            pinMode(SS, INPUT);

            SPCR = 0 | (0 << MSTR) | // Enable master mode
                (polarity << CPOL) | // Set polarity
                (phase << CPHA);     // Set phase; // Enable slave mode
        }
    }

    Atmega328Spi::~Atmega328Spi(){}

    uint8_t Atmega328Spi::transfer(uint8_t data, uint32_t delayMicroS)
    {
        SPDR = data;
        while (!writeComplete()){}
        delayMicroseconds(delayMicroS);
        uint8_t result = SPDR;
        return result;
    }

    void Atmega328Spi::write(uint8_t* buffer,  
                             uint8_t numBytes,
                             uint32_t delayMicroS)
    {
        if (!isMaster_) return;

        selectSlave();
        for (uint8_t i=0; i<numBytes; i++)
        {
            transfer(buffer[i], delayMicroS);
        }
        releaseSlave();
    }

    void Atmega328Spi::read(uint8_t* buffer,
                            uint8_t numBytes,
                            uint32_t delayMicroS)
    {
        if (!isMaster_) return;

        selectSlave();
        for (uint8_t i=0; i<numBytes; i++)
        {
            buffer[i] = transfer(0xff, delayMicroS);
        }
        releaseSlave();
    }

    void Atmega328Spi::writeAndReceive(uint8_t* writeBuffer,
                                       uint8_t* rcvBuffer,
                                       uint8_t numBytes,
                                       uint32_t delayMicroS)
    {
        if (!isMaster_) return;

        selectSlave();
        for (uint8_t i=0; i<numBytes-1; i++)
        {
            rcvBuffer[i] = transfer(writeBuffer[i], delayMicroS);
        }
        releaseSlave();
    }

    void Atmega328Spi::enable()
    {
        SPCR |= bit(SPE);
    }

    void Atmega328Spi::disable()
    {
        SPCR &= ~bit(SPE);
    }

    void Atmega328Spi::reset()
    {
        disable();
        enable();
    }

    bool Atmega328Spi::writeComplete()
    {
        return SPSR & (1 << SPIF);
    }

    bool Atmega328Spi::writeCollisionOccurred()
    {
        return SPSR & (1 << WCOL);
    }

    void Atmega328Spi::setSpiClockSpeed(SpiClock clockSpeed)
    {
        switch (clockSpeed)
        {
            case CLOCK_DIV_2:
                SPCR &= ~bit(SPR0);
                SPCR &= ~bit(SPR1);
                SPSR |= bit(SPI2X);
                break;
            case CLOCK_DIV_4:
                SPCR &= ~bit(SPR0);
                SPCR &= ~bit(SPR1);
                SPSR &= ~bit(SPI2X);
                break;
            case CLOCK_DIV_8:
                SPCR |= bit(SPR0);
                SPCR &= ~bit(SPR1);
                SPSR |= bit(SPI2X);
                break;
            case CLOCK_DIV_16:
                SPCR |= bit(SPR0);
                SPCR &= ~bit(SPR1);
                SPSR &= ~bit(SPI2X);
                break;
            case CLOCK_DIV_32:
                SPCR &= ~bit(SPR0);
                SPCR |= bit(SPR1);
                SPSR |= bit(SPI2X);
                break;
            case CLOCK_DIV_64:
                SPCR &= ~bit(SPR0);
                SPCR |= bit(SPR1);
                SPSR &= ~bit(SPI2X);
                break;
            case CLOCK_DIV_128:
                SPCR |= bit(SPR0);
                SPCR |= bit(SPR1);
                SPSR &= ~bit(SPI2X);
                break;
        }
    }

    void Atmega328Spi::selectSlave()
    {
        digitalWrite(slaveSelect_, LOW);
    }

    void Atmega328Spi::releaseSlave()
    {
        digitalWrite(slaveSelect_, HIGH);
    }

    void Atmega328Spi::setSlaveInterruptCallback(uint8_t slaveSelect, void (*receiveHandler)(uint8_t))
    {
        SlaveSelect = slaveSelect;
        SlaveReceiveHandler = receiveHandler;

        SPCR |= (1 << SPIE); // Enable interrupts
    }
}

ISR(SPI_STC_vect)
{
    uint8_t value = SPDR;
    if (spi::SlaveReceiveHandler == nullptr) return;

    if (digitalRead(spi::SlaveSelect) == LOW)
    {
        spi::SlaveReceiveHandler(value);
    }
}