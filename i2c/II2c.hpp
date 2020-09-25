#ifndef II2C_HPP
#define II2C_HPP

#include <stdint.h>

namespace I2c
{
    class II2c
    {
        public:
            virtual void enable() {}
            virtual void disable() {}

            virtual bool transmit(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes) = 0;
            virtual bool receive(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes) = 0;

            virtual bool transmit(uint8_t slaveId, uint8_t data) = 0;
            virtual bool receive(uint8_t slaveId, uint8_t& data) = 0;
    };
}

#endif