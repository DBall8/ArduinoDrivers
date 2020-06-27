#include "Atmega328AsynchUart.hpp"
#include <Arduino.h>

namespace uart
{

    Atmega328AsynchUart::Atmega328AsynchUart(uint8_t* txBuffer, 
                                             uint8_t* rxBuffer, 
                                             uint8_t bufferSize, 
                                             BaudRate baudRate, 
                                             bool enableParity = false, 
                                             bool polarity = false):
        Atmega328Uart(baudRate, enableParity, polarity),
        txBuffer_(txBuffer, bufferSize, true),
        rxBuffer_(rxBuffer, bufferSize, true),
        size_(bufferSize)
    {
    }

    Atmega328AsynchUart::~Atmega328AsynchUart()
    {}

    void Atmega328AsynchUart::initialize()
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

    void Atmega328AsynchUart::write(uint8_t* buff, uint8_t numBytes)
    {
        for (uint8_t i=0; i<numBytes; i++)
        {
            // Wait until data register is empty (no reading or writing in progress)
            while(!(UCSR0A & (1 << UDRE0))){}

            UDR0 = buff[i];
        }
    }

    void Atmega328AsynchUart::read(uint8_t* buff, uint8_t numBytes)
    {
        for (uint8_t i=0; i<numBytes; i++)
        {
            // Wait until the Read Complete flag is set
            while(!(UCSR0A & (1 << RXC0))){}

            buff[i] = UDR0;
        }
    }
}

// USART RX complete interrupt
// ISR(USART_RX_vect)
// {

// }

// // USART Data Register Empty Interrupt
// ISR(USART_UDRE_vect)
// {

// }