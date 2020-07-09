/**
 * Interface for Analog to Digital converter drivers
 */
#ifndef IADC_HPP
#define IADC_HPP

#include <stdint.h>

namespace adc
{
    class IAdc
    {
        public:
            virtual void enable();
            virtual void disable();
            virtual uint16_t read(bool* success = nullptr) = 0;
    };
}

#endif