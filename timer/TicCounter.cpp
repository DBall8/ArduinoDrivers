#include "TicCounter.hpp"

namespace ticCounter{

    TicCounter::TicCounter():
        ticCount_(0)
    {}
     
    TicCounter::~TicCounter(){}

    uint32_t TicCounter::getTicCount(){
        return ticCount_;
    }

    void TicCounter::incrementTicCount(){
        ticCount_++;
    }
}