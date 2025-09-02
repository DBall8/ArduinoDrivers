#ifndef IPWM_HPP
#define IPWM_HPP

#include <stdint.h>

namespace Pwm
{
    class IPwm
    {
        public:
            virtual void enable() = 0;
            virtual void disable() = 0;

            virtual void setDutyCycle(float percentDuty) = 0;

            virtual uint16_t getResolution(){return 0;}
            virtual void setDutyBinary(uint16_t dutyBinary){}
    };
}

#endif