#ifndef SERVO_HPP
#define SERVO_HPP

#include "drivers/timer/ITimer.hpp"
#include "drivers/dio/IDio.hpp"

namespace Servo
{
    const static int16_t SERVO_MIN_ANGLE = 0;
    const static int16_t SERVO_MAX_ANGLE = 180;
    
    class IServo
    {
        public:
            IServo(Timer::ITimer* pTimer,
                   Dio::IDio* pPin,
                   uint16_t minPeriodMicroS,
                   uint16_t maxPeriodMicroS);
            ~IServo(){}

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

            uint16_t angleToTics(int16_t angle);
    };
}

#endif