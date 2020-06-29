/**
 * Interface for Digital IO Drivers
 */

#ifndef IDIO_HPP
#define IDIO_HPP

#include <stdio.h>

namespace dio
{
    enum Level: uint8_t
    {
        L_LOW = 0,
        L_HIGH
    };

    class IDio
    {
        public:
            virtual bool set(Level level) = 0;
            virtual bool toggle() = 0;
            virtual Level read() = 0;
    };
}

#endif