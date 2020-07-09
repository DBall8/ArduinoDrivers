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
            Atmega328Uart(BaudRate baudRate, uint32_t fCpu, bool enableParity = false, bool polarity = false);
            ~Atmega328Uart();

            virtual void initialize() override;

            // Blocking write
            virtual void write(uint8_t* buff, uint16_t numBytes) override;

            // Blocking read
            virtual uint16_t read(uint8_t* buff, uint16_t numBytes) override;

            void setBaudRate(BaudRate baudRate, uint32_t fCpu);

        protected:
            BaudRate baudRate_;
            uint32_t fCpu_;
            bool enableParity_;
            bool polarity_;

    };
}

#endif