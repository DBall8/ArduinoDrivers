#include "Atmega328Eeprom.hpp"
#include <avr/io.h>

using namespace Interrupt;

namespace Eeprom
{

    Atmega328Eeprom::Atmega328Eeprom(IInterrupt* pIntControl):
        pIntControl_(pIntControl)
    {}

    bool Atmega328Eeprom::write(uint16_t address, uint8_t value)
    {
        // Busy, don't write yet
        if (isBusy()) return false;

        // Set address and data
        EEAR = address;
        EEDR = value;

        // These bits must be set within 4 clock cycles of each other, so disable interrupts
        pIntControl_->pauseInterrupts();

        EECR |= (1 << EEMPE);
        EECR |= (1 << EEPE);

        pIntControl_->resumeInterrupts();
        return true;
    }

    bool Atmega328Eeprom::read(uint16_t address, uint8_t& value)
    {
        // Busy, don't read yet
        while (isBusy()){}

        // Set address to read from
        EEAR = address;

        // These bits must be set within 4 clock cycles of each other, so disable interrupts
        pIntControl_->pauseInterrupts();

        EECR |= (1 << EEMPE);
        EECR |= (1 << EERE);

        pIntControl_->resumeInterrupts();

        // Get the value that was read
        value = EEDR;
        return true;
    }

    bool Atmega328Eeprom::isBusy()
    {
        // Busy if EEPE bit is high
        return (EECR & (1 << EEPE));
    }
}