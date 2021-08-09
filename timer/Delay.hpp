#ifndef DELAY_HPP
#define DELAY_HPP

#include "TicCounter.hpp"
#include "drivers/watchdog/Watchdog.hpp"

#define DELAY(x) Delay::delay(x)
#define DELAY_MICROSECONDS(x) Delay::delayMicroseconds(x)

class Delay
{
    public:

        /**
         * Initialize the delay function with a pointer to a tic counter
         * 
         * @param   pTicCounter Pointer to a tic counter to use for delaying
         */
        static void Initialize(Tic::TicCounter* pTicCounter, Watchdog::IWatchdog* pWdt = nullptr);

        /**
         * Pause for some time
         * 
         * @param   milliseconds    Delay duration in milliseconds
         */
        static void delay(uint32_t milliseconds);
        static void delayMicroseconds(uint32_t microseconds);

    private:
        static Tic::TicCounter* pTicCounter_;
        static Watchdog::IWatchdog* pWdt_;
};

#endif