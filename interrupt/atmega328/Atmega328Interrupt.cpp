#include "Atmega328Interrupt.hpp"
#include <avr/interrupt.h>

namespace Interrupt
{
    Atmega328Interrupt::Atmega328Interrupt():
        areInterruptsEnabled_(false)
    {}

    bool Atmega328Interrupt::areInterruptsEnabled()
    {
        return areInterruptsEnabled_;
    }

    void Atmega328Interrupt::enableInterrupts()
    {
        areInterruptsEnabled_ = true;
        sei();
    }

    void Atmega328Interrupt::disableInterrupts()
    {
        areInterruptsEnabled_ = false;
        cli();
    }
}