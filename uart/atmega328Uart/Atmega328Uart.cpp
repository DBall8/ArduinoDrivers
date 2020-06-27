#include "Atmega328Uart.hpp"
#include "drivers/assert/Assert.hpp"
#include <Arduino.h>

namespace uart
{
    const static long SYS_CLOCK = static_cast<long>(16000000);

    const static uint32_t baudRates[] =
    {
        [BaudRate::BAUD_9600] = 9600,
        [BaudRate::BAUD_115200] = 115200
    };

    const uint32_t BAUD_DIVISOR = 16;
    const uint32_t DOUBLE_BAUD_DIVISOR = 8;

    Atmega328Uart::Atmega328Uart(BaudRate baudRate, bool enableParity, bool polarity):
        polarity_(polarity),
        enableParity_(enableParity),
        baudRate_(baudRate)
    {
    }

    Atmega328Uart::~Atmega328Uart()
    {}

    void Atmega328Uart::initialize()
    {
        setBaudRate(baudRate_);

        // Control register values
        uint8_t ucsrb = 0x00;
        uint8_t ucsrc = 0x00;

        ucsrb |= (1 << RXEN0) | // Enable RX
                 (1 << TXEN0);  // Enable TX

        if (enableParity_) ucsrc |= (1 << UPM01);   // Enable parity (even)
        if (polarity_)     ucsrc |= (1 << UCPOL0);  // Polarity

        ucsrc |= (1 << UCSZ01) | (1<< UCSZ00); // Data frame is 8 bits

        UCSR0B = ucsrb;
        UCSR0C = ucsrc;
    }

    void Atmega328Uart::write(uint8_t* buff, uint8_t numBytes)
    {
        for (uint8_t i=0; i<numBytes; i++)
        {
            // Wait until data register is empty (no reading or writing in progress)
            while(!(UCSR0A & (1 << UDRE0))){}

            UDR0 = buff[i];
        }
    }

    void Atmega328Uart::read(uint8_t* buff, uint8_t numBytes)
    {
        for (uint8_t i=0; i<numBytes; i++)
        {
            // Wait until the Read Complete flag is set
            while(!(UCSR0A & (1 << RXC0))){}

            buff[i] = UDR0;
        }
    }

    void Atmega328Uart::setBaudRate(BaudRate baudRate)
    {
        assert(baudRate < BaudRate::NUM_BAUD_RATES);

        uint32_t baud = baudRates[static_cast<uint8_t>(baudRate)];
        uint32_t ubrr;
        uint8_t ucsra = 0x00;

        switch (baudRate)
        {
            case BAUD_9600:
            {
                // Use double speed (divisor of 8)
                ucsra |= (1 << U2X0); // Double speed bit
                ubrr = (SYS_CLOCK / (DOUBLE_BAUD_DIVISOR * baud)) - 1;
                break;
            }
            default:
            {
                ubrr = (SYS_CLOCK / (BAUD_DIVISOR * baud)) - 1;
                break;
            }
        }

        // Set double speed bit
        UCSR0A = ucsra;

        UBRR0H = (uint8_t)(ubrr >> 8);
        UBRR0L = (uint8_t)ubrr;
    }
}