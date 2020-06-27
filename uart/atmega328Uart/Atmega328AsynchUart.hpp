#ifndef ATMEGA328_ASYNCH_UART_HPP
#define ATMEGA328_ASYNCH_UART_HPP

#include "drivers/uart/atmega328Uart/Atmega328Uart.hpp"
#include "utilities/circular_queue/CircularQueue.hpp"

namespace uart
{
    class Atmega328AsynchUart : public Atmega328Uart
    {
        public:
            Atmega328AsynchUart(uint8_t* txBuffer, 
                                uint8_t* rxBuffer, 
                                uint8_t bufferSize, 
                                BaudRate baudRate, 
                                bool enableParity = false, 
                                bool polarity = false);
            ~Atmega328AsynchUart();

            void initialize() override;

            // Blocking write
            virtual void write(uint8_t* buff, uint8_t numBytes) override;

            // Blocking read
            virtual void read(uint8_t* buff, uint8_t numBytes) override;

        private:
            uint8_t size_;
            CircularQueue<uint8_t> txBuffer_;
            CircularQueue<uint8_t> rxBuffer_;
    };
}

#endif