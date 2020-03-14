#ifndef IRADIO_HPP
#define IRADIO_HPP

#include <stdint.h>

namespace radio
{
    class IRadio
    {
        public:
            virtual void enable() = 0;
            virtual void disable() = 0;

            virtual void setPayloadSize(uint8_t size);

            virtual bool startTransmitting(uint8_t listenerId){};
            virtual bool startReceiving(uint8_t listenerId){};

            virtual bool transmit(uint8_t* buff, uint8_t numBytes) = 0;
            virtual bool isDataAvailable() = 0;
            virtual bool receive(uint8_t* buff, uint8_t numBytes) = 0;
    };
}

#endif