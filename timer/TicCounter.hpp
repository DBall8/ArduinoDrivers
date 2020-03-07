#ifndef TIC_COUNTER_HPP
#define TIC_COUNTER_HPP

#include <stdint.h>

namespace ticCounter{

    class TicCounter{

        public:
            TicCounter();
            ~TicCounter();

            uint32_t getTicCount();
            void incrementTicCount();

        private:
            uint32_t ticCount_;
    };
}

#endif