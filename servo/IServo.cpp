#include "IServo.hpp"

#include "utilities/print/Print.hpp"

using namespace Dio;
using namespace Timer;

namespace Servo
{
    const static uint16_t FULL_PERIOD_MICRO_S = 20000;

    struct ServoInstance
    {
        IServo* pServo;
        bool isHigh;
    };

    const static uint8_t MAX_NUM_SERVOS = 1;
    static ServoInstance ServoInstances[MAX_NUM_SERVOS] = {0};

    static void servoHandleInt()
    {
        if (ServoInstances[0].isHigh)
        {
            ServoInstances[0].pServo->set(Level::L_LOW);
            ServoInstances[0].isHigh = false;
        }
        else
        {
            ServoInstances[0].pServo->set(Level::L_HIGH);
            ServoInstances[0].isHigh = true;
        }
    }

    IServo::IServo(Timer::ITimer* pTimer,
                   Dio::IDio* pPin,
                   uint16_t minPeriodMicroS,
                   uint16_t maxPeriodMicroS):
        pTimer_(pTimer),
        pPin_(pPin),
        minPeriodMicroS_(minPeriodMicroS),
        maxPeriodMicroS_(maxPeriodMicroS)
    {
        pPin_->set(Level::L_LOW);
        pTimer_->disable();
        pTimer_->setInterrupt(&servoHandleInt);

        currAngle_ = 0;
        setAngle(currAngle_);
        totalPeriodTics_ = pTimer_->microSecondsToTics(FULL_PERIOD_MICRO_S);

        ServoInstances[0].isHigh = false;
        ServoInstances[0].pServo = this;
    }

    void IServo::enable()
    {
        pTimer_->enable();
    }

    void IServo::disable()
    {
        pPin_->set(L_LOW);
        pTimer_->disable();
    }

    void IServo::setAngle(int16_t angle)
    {
        if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
        if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;

        currAngle_ = angle;
        onPeriodTics_ = angleToTics(angle);
    }

    uint16_t IServo::angleToTics(int16_t angle)
    {
        uint32_t perdioMicroS = (((uint32_t)angle * (maxPeriodMicroS_ - minPeriodMicroS_)) / SERVO_MAX_ANGLE) + minPeriodMicroS_;
        return pTimer_->microSecondsToTics(perdioMicroS);
    }

    void IServo::set(Dio::Level level)
    {
        if (level == Level::L_HIGH)
        {
            pTimer_->setPeriodTics(onPeriodTics_);
            pTimer_->reset();
            pPin_->set(Level::L_HIGH);
        }
        else
        {
            pPin_->set(Level::L_LOW);
            pTimer_->setPeriodTics(totalPeriodTics_);
        }
    }
}