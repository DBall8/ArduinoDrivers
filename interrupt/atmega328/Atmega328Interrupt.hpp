#ifndef ATMEGA328_INTERRUPT_HPP
#define ATMEGA328_INTERRUPT_HPP

#include <stdint.h>

#include "drivers/interrupt/IInterrupt.hpp"

namespace Interrupt
{
    enum InterruptType : uint8_t
    {
        INT_0 = 0,
        INT_1,

        NUM_INT_TYPES
    };

    enum InterruptTrigger : uint8_t
    {
        LOW = 0,
        CHANGE,
        FALLING,
        RISING,

        NUM_INT_TRIGGERS
    };

    typedef void (*InteruptHandler)(void);

    class Atmega328Interrupt: public IInterrupt
    {
        public:
            Atmega328Interrupt();
            ~Atmega328Interrupt(){}

            bool areInterruptsEnabled() override;
            void enableInterrupts() override;
            void disableInterrupts() override;

            void enableInterrupt(InterruptType type, InterruptTrigger trigger, InteruptHandler intHandler);
            void disableInterrupt(InterruptType type);

        private:
            bool areInterruptsEnabled_;
    };
}

#endif