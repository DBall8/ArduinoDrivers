#include "Atmega328Spi.hpp"
#include "drivers/dio/atmega328/Atmega328Dio.hpp"
#include "drivers/timer/Delay.hpp"
#include <avr/interrupt.h>

using namespace dio;

namespace spi
{
    void Atmega328Spi::SendByte(uint8_t data){ SPDR = data; }
    uint8_t Atmega328Spi::GetByte(){ return SPDR; }

    Atmega328Spi::Atmega328Spi(IDio* pSlaveSelect,
                               bool isMaster,
                               SpiClock clockSpeed,
                               SpiPolarity polarity,
                               SpiPhase phase)
    {
        pSlaveSelect_ = pSlaveSelect;
        isMaster_ = isMaster;

        if (isMaster_)
        {
            Atmega328Dio miso(Port::B, 4, INPUT, L_LOW, false, false);  // MISO must be INPUT
            Atmega328Dio mosi(Port::B, 3, OUTPUT, L_LOW, false, false); // MOSI must be OUTPUT
            Atmega328Dio sck(Port::B, 5, OUTPUT, L_LOW, false, false);  // CLOCK must be OUTPUT
            Atmega328Dio ss(Port::B, 2, OUTPUT, L_LOW, false, false);   // SS might need to be OUTPUT

            releaseSlave();

            setSpiClockSpeed(clockSpeed);

            SPCR = 0 | (1 << MSTR) |    // Enable master mode
                (polarity << CPOL) | // Set polarity
                (phase << CPHA);     // Set phase
            }
        else
        {
            Atmega328Dio miso(Port::B, 4, OUTPUT, L_LOW, false, false); // MISO must be OUTPUT
            Atmega328Dio mosi(Port::B, 3, INPUT, L_LOW, false, false);  // MOSI must be INPUT
            Atmega328Dio sck(Port::B, 5, INPUT, L_LOW, false, false);   // CLOCK must be INPUT
            Atmega328Dio ss(Port::B, 2, INPUT, L_LOW, false, false);    // SS might need to be INPUT

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
        DELAY_MICROSECONDS(delayMicroS);
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
        SPCR |= (0x01 << SPE);
    }

    void Atmega328Spi::disable()
    {
        SPCR &= ~(0x01 << SPE);
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
                SPCR &= ~(0x01 << SPR0);
                SPCR &= ~(0x01 << SPR1);
                SPSR |= (0x01 << SPI2X);
                break;
            case CLOCK_DIV_4:
                SPCR &= ~(0x01 << SPR0);
                SPCR &= ~(0x01 << SPR1);
                SPSR &= ~(0x01 << SPI2X);
                break;
            case CLOCK_DIV_8:
                SPCR |= (0x01 << SPR0);
                SPCR &= ~(0x01 << SPR1);
                SPSR |= (0x01 << SPI2X);
                break;
            case CLOCK_DIV_16:
                SPCR |= (0x01 << SPR0);
                SPCR &= ~(0x01 << SPR1);
                SPSR &= ~(0x01 << SPI2X);
                break;
            case CLOCK_DIV_32:
                SPCR &= ~(0x01 << SPR0);
                SPCR |= (0x01 << SPR1);
                SPSR |= (0x01 << SPI2X);
                break;
            case CLOCK_DIV_64:
                SPCR &= ~(0x01 << SPR0);
                SPCR |= (0x01 << SPR1);
                SPSR &= ~(0x01 << SPI2X);
                break;
            case CLOCK_DIV_128:
                SPCR |= (0x01 << SPR0);
                SPCR |= (0x01 << SPR1);
                SPSR &= ~(0x01 << SPI2X);
                break;
        }
    }

    void Atmega328Spi::selectSlave()
    {
        pSlaveSelect_->set(L_LOW);
    }

    void Atmega328Spi::releaseSlave()
    {
        pSlaveSelect_->set(L_HIGH);
    }

    void Atmega328Spi::setSlaveInterruptCallback(IDio* pSlaveSelect, void (*receiveHandler)(uint8_t))
    {
        pIntSlaveSelect = pSlaveSelect;
        SlaveReceiveHandler = receiveHandler;

        SPCR |= (1 << SPIE); // Enable interrupts
    }
}

ISR(SPI_STC_vect)
{
    uint8_t value = SPDR;
    if (spi::SlaveReceiveHandler == nullptr) return;

    if (spi::pIntSlaveSelect->read() == L_LOW)
    {
        spi::SlaveReceiveHandler(value);
    }
}