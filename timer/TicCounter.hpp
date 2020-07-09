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

        private:
            volatile uint32_t ticCount_;
            uint32_t ticsPerSecond_;
    };
}

#endif