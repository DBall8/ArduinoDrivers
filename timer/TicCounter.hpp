#ifndef TIC_COUNTER_HPP
#define TIC_COUNTER_HPP

#include <stdint.h>

namespace ticCounter{

    class TicCounter{

        public:
            TicCounter(uint32_t ticsPerSecond);
            ~TicCounter();

            volatile uint32_t getTicCount();
            void incrementTicCount();
            uint32_t getTicsPerSecond() { return ticsPerSecond_; }
            uint32_t secondsToTics(uint32_t seconds);
            uint32_t msecondsToTics(uint32_t mseconds);

        private:
            volatile uint32_t ticCount_;
            uint32_t ticsPerSecond_;
    };
}

#endif