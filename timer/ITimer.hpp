#ifndef ITIMER_HPP
#define ITIMER_HPP

#include <stdint.h>

namespace Timer
{
    enum TimerPrescaler: uint8_t
    {
        PRESCALE_OFF = 0,
        PRESCALE_1,
        PRESCALE_8,
        PRESCALE_32,
        PRESCALE_64,
        PRESCALE_128,
        PRESCALE_256,
        PRESCALE_1024,
        EXTERNAL_FALLING,
        EXTERNAL_RISING,
    };

    class ITimer
    {
        public:
            virtual void enable() = 0;
            virtual void disable() = 0;

            virtual void setInterrupt(void (*interrupt)(void));
            virtual void setPeriodTics(uint16_t tics);
            virtual uint16_t microSecondsToTics(uint32_t microseconds);
            virtual void reset();
    };
}

#endif