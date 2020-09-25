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

    uint32_t TicCounter::secondsToTics(uint32_t seconds)
    {
        return ticsPerSecond_ * seconds;
    }
    uint32_t TicCounter::msecondsToTics(uint32_t mseconds)
    {
        return ticsPerSecond_ * mseconds / 1000;
    }
}