#include "TicCounter.hpp"

namespace ticCounter{

    TicCounter::TicCounter(uint32_t ticsPerSecond):
        ticCount_(0),
        ticsPerSecond_(ticsPerSecond)
    {}
     
    TicCounter::~TicCounter(){}

    volatile uint32_t TicCounter::getTicCount(){
        return ticCount_;
    }

    void TicCounter::incrementTicCount(){
        ticCount_++;
    }
}