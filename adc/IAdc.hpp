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
            virtual bool read(uint16_t& value) = 0;
    };
}

#endif