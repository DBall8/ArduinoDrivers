#include "Atmega328Interrupt.hpp"
#include <avr/interrupt.h>

namespace Interrupt
{
    static InteruptHandler intHandlerArray[(uint8_t)NUM_INT_TYPES] = {0};

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

    void Atmega328Interrupt::enableInterrupt(InterruptType type, InterruptTrigger trigger, InteruptHandler intHandler)
    {
        if (((uint8_t)type >= (uint8_t)InterruptType::NUM_INT_TYPES) ||
            ((uint8_t)trigger >= (uint8_t)InterruptTrigger::NUM_INT_TRIGGERS))
        {
            return;
        }

        intHandlerArray[(uint8_t)type] = intHandler;

        switch (type)
        {
            case InterruptType::INT_0:
                EICRA |= ((uint8_t)trigger << ISC00);
                EIMSK |= (1 << INT0);
                break;
            case InterruptType::INT_1:
                EICRA |= ((uint8_t)trigger << ISC10);
                EIMSK |= (1 << INT1);
                break;
            default:
                break;
        }
    }

    void Atmega328Interrupt::disableInterrupt(InterruptType type)
    {
        if ((uint8_t)type >= (uint8_t)InterruptType::NUM_INT_TYPES)
        {
            return;
        }

        switch (type)
        {
            case InterruptType::INT_0:
                EIMSK &= ~(1 << INT0);
                break;
            case InterruptType::INT_1:
                EIMSK &= ~(1 << INT1);
                break;
            default:
                break;
        }

        intHandlerArray[(uint8_t)type] = nullptr;
    }

    static void handleInterrupt(InterruptType type)
    {
        if (intHandlerArray[(uint8_t)type] != nullptr)
        {
            intHandlerArray[(uint8_t)type]();
        }
    }
}

ISR(INT0_vect)
{
    Interrupt::handleInterrupt(Interrupt::InterruptType::INT_0);
}

ISR(INT1_vect)
{
    Interrupt::handleInterrupt(Interrupt::InterruptType::INT_1);
}