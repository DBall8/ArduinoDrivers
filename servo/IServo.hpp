#ifndef SERVO_HPP
#define SERVO_HPP

#include "drivers/timer/ITimer.hpp"
#include "drivers/dio/IDio.hpp"

namespace Servo
{    
    class IServo
    {
        public:
            IServo(Timer::ITimer* pTimer,
                   Dio::IDio* pPin,
                   uint16_t minPeriodMicroS,
                   uint16_t maxPeriodMicroS,
                   int16_t minAngle,
                   int16_t maxAngle);
            ~IServo(){}

            int16_t getMinAngle();
            int16_t getMaxAngle();

            void enable();
            void disable();

            void setAngle(int16_t angle);
            int16_t getAngle() { return currAngle_; }

            void set(Dio::Level level);
            
        private:
            Timer::ITimer* pTimer_;
            Dio::IDio* pPin_;
            int16_t currAngle_;
            uint16_t onPeriodTics_;
            uint16_t totalPeriodTics_;
            uint16_t minPeriodMicroS_;
            uint16_t maxPeriodMicroS_;
            int16_t minAngle_;
            int16_t maxAngle_;

            uint16_t angleToTics(int16_t angle);
    };
}

#endif