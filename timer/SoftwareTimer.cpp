#include "SoftwareTimer.hpp"

#include "utilities/print/Print.hpp"

using namespace Tic;
using namespace Watchdog;

namespace Timer{

    SoftwareTimer::SoftwareTimer(uint32_t periodInTics,
                                 TicCounter* pTicCounter,
                                 IWatchdog* pWdt):
        pTicCounter_(pTicCounter),
        pWdt_(pWdt),
        periodInTics_(periodInTics),
        isEnabled_(false),
        numPeriodsPassed_(0),
        hasOneShotPassed_(false),
        startTic_(0),
        prevPeriodTic_(0)
    {
    }

    SoftwareTimer::~SoftwareTimer(){

    }

    void SoftwareTimer::enable(){
        isEnabled_ = true;
        
        hasOneShotPassed_ = false;
        numPeriodsPassed_ = 0;

        startTic_ = pTicCounter_->getTicCount();
        prevPeriodTic_ = startTic_;
    }
    
    void SoftwareTimer::disable()
    {
        isEnabled_ = false;
    }
    void SoftwareTimer::reset(){
        enable();
    }

    void SoftwareTimer::setPeriod(uint32_t periodInTics){
        periodInTics_ = periodInTics;

        if (isEnabled_){
            reset();
        }
    }

    void SoftwareTimer::setPeriodMs(uint32_t periodInMs)
    {
        setPeriod(pTicCounter_->msecondsToTics(periodInMs));
    }

    void SoftwareTimer::setPeriodS(uint32_t periodInS)
    {
        setPeriod(pTicCounter_->secondsToTics(periodInS));
    }

    bool SoftwareTimer::isEnabled(){
        return isEnabled_;
    }

    bool SoftwareTimer::hasPeriodPassed(){
        if (!isEnabled_) return false;
        update();
        bool hasPeriodPassed = numPeriodsPassed_ > 0;

        // If period has passed, find the next period
        if (hasPeriodPassed){
            // Calculate the new "previous period" tic
            prevPeriodTic_ += numPeriodsPassed_ * periodInTics_;
            numPeriodsPassed_ = 0;
        }

        return hasPeriodPassed;
    }

    bool SoftwareTimer::hasOneShotPassed(){
        if (!isEnabled_) return false;
        update();

        return hasOneShotPassed_;
    }

    void SoftwareTimer::update(){

        uint32_t currentTic = pTicCounter_->getTicCount();

        // If one shot hasn't passed, check if it now has
        if (!hasOneShotPassed_)
        {
            hasOneShotPassed_ = (currentTic - startTic_) >= periodInTics_;
        }

        // If period hasn't passed, check if it has
        numPeriodsPassed_ += (currentTic - prevPeriodTic_) / periodInTics_;

        // Nourish watchdog if one was given
        if (pWdt_ != nullptr) pWdt_->reset();

        // if (hasPeriodPassed_)
        // {
        //     PRINTLN("    Ctic: 0x%x%x, Ptic 0x%x%x, Period: 0x%x%x",
        //         (uint16_t)(currentTic >> 16),
        //         (uint16_t)currentTic,
        //         (uint16_t)(prevPeriodTic_ >> 16),
        //         (uint16_t)prevPeriodTic_,
        //         (uint16_t)(periodInTics_ >> 16),
        //         (uint16_t)periodInTics_);
        // }
    }

    void SoftwareTimer::printDebug()
    {
        // Serial.print("Current tic: ");
        // Serial.print(pTicCounter_->getTicCount());
        // Serial.print(" Prev tic count: ");
        // Serial.print(prevPeriodTic_);
        // Serial.print( "Period In tics: ");
        // Serial.println(periodInTics_);
    }
}