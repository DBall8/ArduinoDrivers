#ifndef ISPI_HPP
#define ISPI_HPP

#include <stdint.h>

namespace Spi
{

    enum SpiPolarity: uint8_t
    {
        IDLE_LOW = 0,
        IDLE_HIGH,
    };

    enum SpiPhase: uint8_t
    {
        SAMPLE_LEADING = 0,
        SAMPLE_FALLING
    };

    class ISpi
    {
        public:
            ISpi(){}
            ~ISpi(){}

            virtual uint8_t transfer(uint8_t data, uint32_t delayMicroS) = 0;

            virtual void write(uint8_t* buffer,
                               uint8_t numBytes,
                               uint32_t delayMicroS) = 0;

            virtual void read(uint8_t* buffer,
                              uint8_t numBytes,
                              uint32_t delayMicroS) = 0;

            virtual void writeAndReceive(uint8_t* writeBuffer,
                                         uint8_t* rcvBuffer,
                                         uint8_t numBytes,
                                         uint32_t delayMicroS) = 0;

            virtual void selectSlave() {}
            virtual void releaseSlave() {}

            virtual void enable() = 0;
            virtual void disable() = 0;
            virtual void reset() = 0;
    };
}

#endif