#ifndef SOFTWARE_TIMER_HPP
#define SOFTWARE_TIMER_HPP

#include "TicCounter.hpp"
#include <stdint.h>

using namespace ticCounter;

namespace softwareTimer{

    class SoftwareTimer{
        public:
            SoftwareTimer(uint32_t periodTics, TicCounter* pTicCounter);
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
            TicCounter* pTicCounter_;   // Tic tracker
            uint32_t periodInTics_;     // Timer period
            bool isEnabled_;            
            bool hasPeriodPassed_;      // Has at least one period passed
            bool hasOneShotPassed_;     // Has a new period passed since last check

            uint32_t startTic_;         // Tic the timer was enabled during
            uint32_t prevPeriodTic_;    // Tic of the most recent period that passed

            void findNextPeriodTic();   // Calculate the most recent period's tic
            void update();              // Check if a period has passed
    };
}

#endif