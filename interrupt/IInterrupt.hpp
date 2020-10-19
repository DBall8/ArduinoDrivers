#ifndef IINTERRUPT_HPP
#define IINTERRUPT_HPP

namespace Interrupt
{
    class IInterrupt
    {
        public:
            virtual bool areInterruptsEnabled() = 0;
            virtual void enableInterrupts() = 0;
            virtual void disableInterrupts() = 0;
    };
}

#endif