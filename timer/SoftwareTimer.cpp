#include "SoftwareTimer.hpp"

namespace softwareTimer{

    SoftwareTimer::SoftwareTimer(uint32_t periodInTics, TicCounter* pTicCounter):
        pTicCounter_(pTicCounter),
        periodInTics_(periodInTics),
        isEnabled_(false),
        hasPeriodPassed_(false),
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
        hasPeriodPassed_ = false;

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
        if (!isEnabled_){
            periodInTics_ = periodInTics;
        }
    }

    bool SoftwareTimer::isEnabled(){
        return isEnabled_;
    }

    bool SoftwareTimer::hasPeriodPassed(){
        update();
        bool hasPeriodPassed = hasPeriodPassed_;

        // If period has passed, find the next period
        if (hasPeriodPassed_){
            // Calculate the new "previous period" tic
            uint32_t ticCount = pTicCounter_->getTicCount();
            uint32_t periodOffset = (ticCount - startTic_) % periodInTics_;
            prevPeriodTic_ = (ticCount - periodOffset);
            hasPeriodPassed_ = false;
        }

        return hasPeriodPassed;
    }

    bool SoftwareTimer::hasOneShotPassed(){
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
        if (!hasPeriodPassed_)
        {
            hasPeriodPassed_ = (currentTic - prevPeriodTic_) >= periodInTics_;
        }
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