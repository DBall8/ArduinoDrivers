#include "Atmega328Dio.hpp"
#include "drivers/assert/Assert.hpp"

#include <avr/io.h>

namespace Dio
{
    // PORT registers
    static volatile uint8_t* PORT_REGS[] = 
    {
        [static_cast<uint8_t>(Port::B)] = &PORTB,
        [static_cast<uint8_t>(Port::C)] = &PORTC,
        [static_cast<uint8_t>(Port::D)] = &PORTD,
    };

    // Direction registers
    static volatile uint8_t* DIRECION_REGS[] = 
    {
        [static_cast<uint8_t>(Port::B)] = &DDRB,
        [static_cast<uint8_t>(Port::C)] = &DDRC,
        [static_cast<uint8_t>(Port::D)] = &DDRD,
    };

    // PIN registers
    static volatile uint8_t* PIN_REGS[] = 
    {
        [static_cast<uint8_t>(Port::B)] = &PINB,
        [static_cast<uint8_t>(Port::C)] = &PINC,
        [static_cast<uint8_t>(Port::D)] = &PIND,
    };

    Atmega328Dio::Atmega328Dio(Port port, 
                               uint8_t pin,
                               Mode mode,
                               Level initialLevel,
                               bool inverted,
                               bool usePullup):
        port_(port),
        pin_(pin),
        mode_(mode),
        inverted_(inverted)
    {
        assert(pin_ < NUM_PINS);

        if (mode_ == INPUT)
        {
            // When set as input, PORT register is used to designate pullup
            // HIGH = pullup
            // LOW = no pullup
            set(usePullup ? L_HIGH : L_LOW);
        }
        else if (mode_ == OUTPUT)
        {
            set(initialLevel);
        }
        else
        {
            assert(false);
        }

        setDirection(mode_);
    }

    bool Atmega328Dio::set(Level level)
    {
        // Flip the input if this pin is inverted
        if (inverted_) level = (level == L_HIGH) ? L_LOW : L_HIGH;

        if (level == L_LOW)
        {
            *PORT_REGS[static_cast<uint8_t>(port_)] &= ~(0x01 << pin_);
            return true;
        }
        
        if (level == L_HIGH)
        {
            *PORT_REGS[static_cast<uint8_t>(port_)] |= (0x01 << pin_);
            return true;
        }

        return false;
    }

    bool Atmega328Dio::toggle()
    {
        *PIN_REGS[static_cast<uint8_t>(port_)] |= (0x01 << pin_);
        return true;
    }

    Level Atmega328Dio::read()
    {
        bool value = *PIN_REGS[static_cast<uint8_t>(port_)] & (0x01 << pin_);
        return value ? Level::L_HIGH : Level::L_LOW;
    }

    void Atmega328Dio::setDirection(Mode mode)
    {
        if (mode  == INPUT)
        {
            *DIRECION_REGS[static_cast<uint8_t>(port_)] &= ~(0x01 << pin_);
        }
        else if (mode == OUTPUT)
        {
            *DIRECION_REGS[static_cast<uint8_t>(port_)] |= (0x01 << pin_);
        }
        else
        {
            assert(false);
        }
    }

}