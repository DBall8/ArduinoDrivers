#ifndef ISERIAL_HPP
#define ISERIAL_HPP

#include <stdint.h>

namespace Serial
{
    enum BaudRate: uint8_t
    {
        BAUD_9600 = 0,
        BAUD_115200,
        NUM_BAUD_RATES
    };

    class ISerial
    {
        public:
            virtual void initialize(){};
            virtual bool isDataAvailable() { return false; }
            virtual void write(uint8_t* buff, uint16_t numBytes) = 0;
            virtual uint16_t read(uint8_t* buff, uint16_t numBytes) = 0;
            virtual void flush(){}
    };
}

#endif