#ifndef ATMEGA328_INTERRUPT_HPP
#define ATMEGA328_INTERRUPT_HPP

#include "drivers/interrupt/IInterrupt.hpp"

namespace interrupt
{
    class Atmega328Interrupt: public IInterrupt
    {
        public:
            Atmega328Interrupt();
            ~Atmega328Interrupt(){}

            bool areInterruptsEnabled() override;
            void enableInterrupts() override;
            void disableInterrupts() override;

        private:
            bool areInterruptsEnabled_;
    };
}

#endif