#ifndef SOFTWARE_TIMER_HPP
#define SOFTWARE_TIMER_HPP

#include "TicCounter.hpp"
#include <stdint.h>

#include "drivers/watchdog/Watchdog.hpp"

namespace Timer{

    class SoftwareTimer{
        public:
            SoftwareTimer(uint32_t periodTics,
                          Tic::TicCounter* pTicCounter,
                          Watchdog::IWatchdog* pWdt = nullptr);
            ~SoftwareTimer();

            void enable();
            void disable();
            void reset();

            void setPeriod(uint32_t periodInTics);

            bool isEnabled();
            bool hasPeriodPassed();
            bool hasOneShotPassed();

            void printDebug();

        private:
            Tic::TicCounter* pTicCounter_;  // Tic tracker
            Watchdog::IWatchdog* pWdt_;     // Watchdog time to nourish, if one was given
            uint32_t periodInTics_;     // Timer period
            bool isEnabled_;            
            uint16_t numPeriodsPassed_; // Number of periods passed since last call to hasPeriodPassed
            bool hasOneShotPassed_;     // Has a new period passed since last check

            uint32_t startTic_;         // Tic the timer was enabled during
            uint32_t prevPeriodTic_;    // Tic of the most recent period that passed

            void findNextPeriodTic();   // Calculate the most recent period's tic
            void update();              // Check if a period has passed
    };
}

#endif