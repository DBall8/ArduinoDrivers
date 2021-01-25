#include "Atmega328Dio.hpp"
#include "drivers/assert/Assert.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

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

    // Data needed to set a pin change interrupt for each port
    struct PcintPortInfo
    {
        uint8_t offset;                 // Pin offset for the first pin of this port
        volatile uint8_t portEnableBit; // Bit for enabling/disabling interrupts on this port
        volatile uint8_t* pinEnableReg; // Register for enabling/disabling interrupts per pin
    };

    // PCINT port info
    const static PcintPortInfo PCINT_PORT_INFO[] =
    {
        [static_cast<uint8_t>(Port::B)] = {.offset = 0, .portEnableBit = PCIE0, .pinEnableReg = &PCMSK0},
        [static_cast<uint8_t>(Port::C)] = {.offset = 8, .portEnableBit = PCIE1, .pinEnableReg = &PCMSK1},
        [static_cast<uint8_t>(Port::D)] = {.offset = 16, .portEnableBit = PCIE2, .pinEnableReg = &PCMSK2},
    };

    // Static array of function pointers to handle PCINTs for each port
    const static uint8_t NUM_PCINT_PORTS = 3;
    static void (*pcintPortCallbacks[NUM_PCINT_PORTS])() = { nullptr, nullptr, nullptr};

    // Interrupt handler for port B pin changes
    ISR(PCINT0_vect)
    {
        if (pcintPortCallbacks[0] != nullptr)
        {
            pcintPortCallbacks[0]();
        }
    }

    // Interrupt handler for port C pin changes
    ISR(PCINT1_vect)
    {
        if (pcintPortCallbacks[1] != nullptr)
        {
            pcintPortCallbacks[1]();
        }
    }

    // Interrupt handler for port D pin changes
    ISR(PCINT2_vect)
    {
        if (pcintPortCallbacks[2] != nullptr)
        {
            pcintPortCallbacks[2]();
        }
    }

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
        else
        {
            *PORT_REGS[static_cast<uint8_t>(port_)] |= (0x01 << pin_);
            return true;
        }

        return false;
    }

    void Atmega328Dio::set(uint8_t level)
    {
        if (level)
        {
            *PORT_REGS[static_cast<uint8_t>(port_)] |= (0x01 << pin_);
        }
        else
        {
            *PORT_REGS[static_cast<uint8_t>(port_)] &= ~(0x01 << pin_);
        }
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

    uint8_t Atmega328Dio::getPcintNumber(Port port, uint8_t pin)
    {
        return PCINT_PORT_INFO[(uint8_t)port].offset + pin;
    }

    void Atmega328Dio::enableInterrupt(void (*pIntHandler)(void))
    {
        // If a callback was given, set it as the callback for this port
        // There is only one callback per port, add logic to the callback to differentiate between pins
        if (pIntHandler != nullptr)
        {
            pcintPortCallbacks[(uint8_t)port_] = pIntHandler;
        }

        // Get information about the port and the pin
        PcintPortInfo pcintPortInfo = PCINT_PORT_INFO[(uint8_t)port_];

        // Enable pin change interrupts for this port
        PCICR |= (1 << pcintPortInfo.portEnableBit);

        // Enable pin change interrupts for this pin
        *(pcintPortInfo.pinEnableReg) |= (1 << pin_);
    }

    void Atmega328Dio::disableInterrupt()
    {
        // Get information about the port and the pin
        PcintPortInfo pcintPortInfo = PCINT_PORT_INFO[(uint8_t)port_];
        uint8_t pinBit = pin_ - pcintPortInfo.offset;

        // Disable pin change interrupts for this pin
        *(pcintPortInfo.pinEnableReg) &= ~(1 << pinBit);
    }

}