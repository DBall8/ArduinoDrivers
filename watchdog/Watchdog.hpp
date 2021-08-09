#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

#include <stdint.h>

enum class ResetCause : uint8_t
{
    WATCHDOG,
    BROWN_OUT,
    POWER_ON
};

namespace Watchdog
{
    class IWatchdog
    {
        public:
            virtual void init();
            virtual void enable() = 0;
            virtual void disable() = 0;
            virtual void setTimeout(uint32_t timeoutMs) = 0;
            virtual void reset() = 0;

            virtual ResetCause getResetCause()
            {
                return ResetCause::POWER_ON;
            }
    };
}

#endif