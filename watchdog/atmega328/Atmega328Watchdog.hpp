#ifndef ATMEGA328_WATCHDOG_HPP
#define ATMEGA328_WATCHDOG_HPP

#include "drivers/watchdog/Watchdog.hpp"

namespace Watchdog
{
    class Atmega328Watchdog : public IWatchdog
    {
        /**
         * Warning: If a WDT reset occurs, it will stay enabled but reset to minimum reset time
         * Therefore we must always disable the WDT on startup to avoid triggering infinite reset loops
         */
        public:
            Atmega328Watchdog();
            ~Atmega328Watchdog();

            void init() override;
            void enable() override;
            void disable() override;
            void setTimeout(uint32_t timeoutMs) override;
            void reset() override;

            ResetCause getResetCause();

        private:
            uint8_t prescaler_;
            ResetCause resetCause_;
            bool isEnabled_;

            void applyChange();
    };
}

#endif