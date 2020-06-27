#ifndef IUART_HPP
#define IUART_HPP

#include <stdint.h>

namespace uart
{
    class IUart
    {
        public:
            virtual void initialize(){};
            virtual void write(uint8_t* buff, uint8_t numBytes) = 0;
            virtual void read(uint8_t* buff, uint8_t numBytes) = 0;
    };
}

#endif