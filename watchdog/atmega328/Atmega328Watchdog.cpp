#include "Atmega328Watchdog.hpp"

#include "drivers/assert/Assert.hpp"
#include <avr/wdt.h>
#include <avr/interrupt.h>

namespace Watchdog
{
    // Index corresponds to the prescaler value, entry corresponds to timeout in MS
    const static uint32_t TIMEOUTS_MS[] = 
    {
        [0x00] = 16,
        [0x01] = 32,
        [0x02] = 64,
        [0x03] = 125,
        [0x04] = 250,
        [0x05] = 500,
        [0x06] = 1000,
        [0x07] = 2000,
        [0x08] = 4000,
        [0x09] = 8000,
    };
    const static uint8_t NUM_PRESCALERS = sizeof(TIMEOUTS_MS) / sizeof(TIMEOUTS_MS[0]);

    Atmega328Watchdog::Atmega328Watchdog():
        prescaler_(0),
        resetCause_(ResetCause::POWER_ON),
        isEnabled_(false)
    {}

    Atmega328Watchdog::~Atmega328Watchdog(){}

    void Atmega328Watchdog::init()
    {
        if (MCUSR & (1 << WDRF))
        {
            resetCause_ = ResetCause::WATCHDOG;
        }
        else if (MCUSR & (1 << BORF))
        {
            resetCause_ = ResetCause::BROWN_OUT;
        }
        else
        {
            resetCause_ = ResetCause::POWER_ON;
        }

        // Reset register so that we can find the source of the next reset
        MCUSR = 0;

        // Assume WDT was enabled on startup, and disable now until we purposefully enable it
        isEnabled_ = true;
        disable();
    }

    void Atmega328Watchdog::enable()
    {
        if (!isEnabled_)
        {
            isEnabled_ = true;
            applyChange();
        }
    }
    void Atmega328Watchdog::disable()
    {
        if (isEnabled_)
        {
            isEnabled_ = false;
            applyChange();
        }
    }

    void Atmega328Watchdog::setTimeout(uint32_t timeoutMs)
    {
        // Find the closest timeout available without going under
        for (uint8_t i=0; i<NUM_PRESCALERS; i++)
        {
            if (TIMEOUTS_MS[i] >= timeoutMs)
            {
                prescaler_ = i;
                // Only apply the change if we are disabled, otherwise we do not need to
                if (isEnabled_) applyChange();
                return;
            }
        }

        // Cannot set a timeout this long
        assert(false, "Cannot set watchdog timeout this high.");
    }

    void Atmega328Watchdog::reset()
    {
        wdt_reset();
    }

    ResetCause Atmega328Watchdog::getResetCause()
    {
        return resetCause_;
    }

    void Atmega328Watchdog::applyChange()
    {
        uint8_t newState = 0;
        if (isEnabled_)
        {
            // Set the enabled bit
            newState |= (1 << WDE);

            uint8_t prescaler = prescaler_;
            if (prescaler & 0x08)
            {
                // Set highest prescaler bit and clear it from our local copy
                newState |= (1 << WDP3);
                prescaler &= ~(0x08);
            }

            // Add the remaining prescaler bits
            newState |= prescaler;
        }

        cli();
        wdt_reset();
        // Set these flags high to apply changes
        WDTCSR |= (1 << WDCE) | (1 << WDE);
        WDTCSR = newState;
        sei();
    }
}