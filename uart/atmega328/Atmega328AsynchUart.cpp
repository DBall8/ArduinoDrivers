#include "Atmega328AsynchUart.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

using namespace Interrupt;

namespace Uart
{

    const static char* BUFF_FULL_STR = "\r\nTX BUFF FULL\r\n";
    const static uint8_t BUFF_FULL_LEN = 16;

    CircularQueue<uint8_t>* Atmega328AsynchUart::pTxBuffer_ = nullptr;
    CircularQueue<uint8_t>* Atmega328AsynchUart::pRxBuffer_ = nullptr;
    IInterrupt* Atmega328AsynchUart::pInterruptControl_ = nullptr;

    Atmega328AsynchUart::Atmega328AsynchUart(uint8_t* txBuffer, 
                                             uint8_t* rxBuffer, 
                                             uint16_t txLength,
                                             uint16_t rxLength, 
                                             BaudRate baudRate,
                                             uint32_t fCpu,
                                             IInterrupt* pInterruptControl, 
                                             bool enableParity, 
                                             bool polarity):
        Atmega328Uart(baudRate, fCpu, enableParity, polarity),
        txBuffer_(txBuffer, txLength, false),
        rxBuffer_(rxBuffer, rxLength, false),
        txLength_(txLength),
        rxLength_(rxLength),
        txBufferFull_(false)
    {
        Atmega328AsynchUart::pTxBuffer_ = &txBuffer_;
        Atmega328AsynchUart::pRxBuffer_ = &rxBuffer_;
        Atmega328AsynchUart::pInterruptControl_ = pInterruptControl;
    }

    Atmega328AsynchUart::~Atmega328AsynchUart()
    {}

    void Atmega328AsynchUart::initialize()
    {
        setBaudRate(baudRate_, fCpu_);

        // Control register values
        uint8_t ucsrb = 0x00;
        uint8_t ucsrc = 0x00;

        ucsrb |= (1 << RXCIE0) | // Enable RX interrupts
                 (1 << UDRIE0) | // Enable TX interrupts
                 (1 << RXEN0)  | // Enable RX
                 (1 << TXEN0);   // Enable TX

        if (enableParity_) ucsrc |= (1 << UPM01);   // Enable parity (even)
        if (polarity_)     ucsrc |= (1 << UCPOL0);  // Polarity

        ucsrc |= (1 << UCSZ01) | (1<< UCSZ00); // Data frame is 8 bits

        UCSR0B = ucsrb;
        UCSR0C = ucsrc;
    }

    void Atmega328AsynchUart::write(uint8_t* buff, uint16_t numBytes)
    {
        // If the tx buffer is full, don't add any more data to write until
        // it empties again
        if (txBufferFull_)
        {
            if (txBuffer_.isEmpty())
            {
                txBufferFull_ = false;
            }
            return;
        }

        // Add data to tx buffer
        bool buffFull = false;
        pInterruptControl_->disableInterrupts();
        for (uint16_t i=0; i<numBytes; i++)
        {
            if (txBuffer_.length() >= (txLength_ - BUFF_FULL_LEN))
            {
                // Buffer is full (minus warning string length)
                buffFull = true;
                break;
            }

            txBuffer_.push(buff[i]);
        }

        if (buffFull)
        {
            // Buffer is full, put the warning string at the end to show that it overfilled
            txBufferFull_ = true;
            for (uint16_t i=0; i<BUFF_FULL_LEN; i++)
            {
                txBuffer_.push(BUFF_FULL_STR[i]);
            }
        }

        // No write in progress, we can start one now
        if (!txBuffer_.isEmpty() && (UCSR0A & (1 << UDRE0)))
        {
            UDR0 = txBuffer_.pop();
        }

        pInterruptControl_->enableInterrupts();
    }

    uint16_t Atmega328AsynchUart::read(uint8_t* buff, uint16_t numBytes)
    {
        pInterruptControl_->disableInterrupts();
        for (uint16_t i=0; i<numBytes; i++)
        {
            if (rxBuffer_.isEmpty())
            {
                pInterruptControl_->enableInterrupts();
                return i;
            }

            buff[i] = rxBuffer_.pop();
        }

        pInterruptControl_->enableInterrupts();
        return numBytes;
    }

    bool Atmega328AsynchUart::isDataAvailable()
    {
        return !rxBuffer_.isEmpty();
    }

    void Atmega328AsynchUart::HanleDataRegisterEmpty()
    {
        if (!Atmega328AsynchUart::pTxBuffer_->isEmpty())
        {
            //Atmega328AsynchUart::pInterruptControl_->disableInterrupts();
            uint8_t value = Atmega328AsynchUart::pTxBuffer_->pop();
            //Atmega328AsynchUart::pInterruptControl_->enableInterrupts();
            UDR0 = value;
        }
    }

    void Atmega328AsynchUart::HanleRxDataAvailable()
    {
        Atmega328AsynchUart::pRxBuffer_->push(UDR0);
    }
}



// USART RX complete interrupt
ISR(USART_RX_vect)
{
    Uart::Atmega328AsynchUart::HanleRxDataAvailable();
}

// // USART Data Register Empty Interrupt
ISR(USART_UDRE_vect)
{
    Uart::Atmega328AsynchUart::HanleDataRegisterEmpty();
}