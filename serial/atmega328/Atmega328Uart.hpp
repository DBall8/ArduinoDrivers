#ifndef ATMEGA328_UART_HPP
#define ATMEGA328_UART_HPP

#include "drivers/serial/ISerial.hpp"

namespace SerialComm
{
    class Atmega328Uart : public ISerial
    {
        public:
            Atmega328Uart(BaudRate baudRate, uint32_t fCpu, bool enableParity = false, bool polarity = false);
            ~Atmega328Uart();

            virtual void initialize() override;

            // Blocking write
            virtual void write(const uint8_t* buff, uint16_t numBytes) override;

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