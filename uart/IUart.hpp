#ifndef IUART_HPP
#define IUART_HPP

#include <stdint.h>

namespace uart
{
    class IUart
    {
        public:
            virtual void initialize(){};
            virtual bool isDataAvailable() { return false; }
            virtual void write(uint8_t* buff, uint16_t numBytes) = 0;
            virtual uint16_t read(uint8_t* buff, uint16_t numBytes) = 0;
    };
}

#endif