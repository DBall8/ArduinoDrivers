#ifndef ATMEGA328_UART_HPP
#define ATMEGA328_UART_HPP

#include "drivers/uart/IUart.hpp"

namespace uart
{
    enum BaudRate: uint8_t
    {
        BAUD_9600 = 0,
        BAUD_115200,
        NUM_BAUD_RATES
    };

    class Atmega328Uart : public IUart
    {
        public:
            Atmega328Uart(BaudRate baudRate, bool enableParity = false, bool polarity = false);
            ~Atmega328Uart();

            virtual void initialize() override;

            // Blocking write
            virtual void write(uint8_t* buff, uint8_t numBytes) override;

            // Blocking read
            virtual void read(uint8_t* buff, uint8_t numBytes) override;

            void setBaudRate(BaudRate baudRate);

        protected:
            bool enableParity_;
            bool polarity_;
            BaudRate baudRate_;
    };
}

#endif