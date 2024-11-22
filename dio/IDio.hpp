/**
 * Interface for Digital IO Drivers
 */

#ifndef IDIO_HPP
#define IDIO_HPP

#include <stdint.h>

namespace Dio
{
    enum Level: uint8_t
    {
        L_LOW = 0,
        L_HIGH
    };

    enum Mode: uint8_t
    {
        INPUT = 0,
        OUTPUT
    };

    class IDio
    {
        public:
            virtual bool set(Level level) = 0;
            virtual void set(uint8_t level){ set(level ? Level::L_HIGH : Level::L_LOW);};
            virtual bool toggle() = 0;
            virtual Level read() = 0;
            virtual void setOutputMode(Level level) = 0;
            virtual void setInputMode(bool usePullup) = 0;

            virtual void enableInterrupt(void (*pIntHandler)(void) = nullptr) {}
            virtual void disableInterrupt() {}
    };
}

#endif