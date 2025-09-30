#include "BipolarStepperMotor.hpp"
#include "drivers/assert/Assert.hpp"
#include "utilities/print/Print.hpp"

using namespace Dio;

namespace StepperMotor
{
    const static uint16_t COUNT_MAX = 1000;

    const static int NUM_STEPS = 8;
    const static Level FullStepDriveTable[NUM_STEPS][BipolarStepperMotor::NUM_PINS] =
    {
        {L_LOW, L_HIGH, L_HIGH, L_LOW},
        {L_LOW, L_LOW, L_HIGH, L_HIGH},
        {L_LOW, L_LOW, L_HIGH, L_HIGH},
        {L_HIGH, L_LOW, L_LOW, L_HIGH},
        {L_HIGH, L_LOW, L_LOW, L_HIGH},
        {L_HIGH, L_HIGH, L_LOW, L_LOW},
        {L_HIGH, L_HIGH, L_LOW, L_LOW},
        {L_LOW, L_HIGH, L_HIGH, L_LOW},
    };

    const static Level HalfStepDriveTable[NUM_STEPS][BipolarStepperMotor::NUM_PINS] =
    {
        {L_HIGH, L_LOW, L_LOW, L_HIGH},
        {L_HIGH, L_LOW, L_LOW, L_LOW},
        {L_HIGH, L_HIGH, L_LOW, L_LOW},
        {L_LOW, L_HIGH, L_LOW, L_LOW},
        {L_LOW, L_HIGH, L_HIGH, L_LOW},
        {L_LOW, L_LOW, L_HIGH, L_LOW},
        {L_LOW, L_LOW, L_HIGH, L_HIGH},
        {L_LOW, L_LOW, L_LOW, L_HIGH},
    };

    BipolarStepperMotor::BipolarStepperMotor(
        Timer::ITimer* pTimer,
        Dio::IDio* pPinA,
        Dio::IDio* pPinB,
        Dio::IDio* pPinC,
        Dio::IDio* pPinD
    ):
        pTimer_(pTimer),
        mode_(FULL_STEP),
        currStep_(0),
        updateCount_(0),
        forward_(true),
        count_(0)
    {
        pPins_[0] = pPinA;
        pPins_[1] = pPinB;
        pPins_[2] = pPinC;
        pPins_[3] = pPinD;

        if ((pTimer == nullptr) ||
            (pPins_[0] == nullptr) ||
            (pPins_[1] == nullptr) ||
            (pPins_[2] == nullptr) ||
            (pPins_[3] == nullptr))
        {
            assert(false, "Step motor init");
        }

        stop();
    }

    void BipolarStepperMotor::setMode(StepperMode mode)
    {
        mode_ = mode;
    }

    StepperMode BipolarStepperMotor::getMode()
    {
        return mode_;
    }

    void BipolarStepperMotor::stop()
    {
        pTimer_->disable();
    }

    void BipolarStepperMotor::run(float speed)
    {
        if (speed == 0)
        {
            stop();
            return;
        }

        forward_ = speed > 0;
        if (speed < 0) speed *= -1;
        updateCount_ = (COUNT_MAX / (speed * 10))- 1;

        setPins();
        pTimer_->enable();
    }

    void BipolarStepperMotor::powerDown()
    {
        stop();

        uint8_t i = 0;
        for (i = 0; i < NUM_PINS; i++)
        {
            pPins_[i]->set(L_LOW);
        }
    }

    void BipolarStepperMotor::interruptHandler()
    {
        count_++;
        if (count_ >= COUNT_MAX) count_ = 0;
        if (count_ < updateCount_) return;

        // Move to next step, looping back to 0 after the last step
        count_ = 0;
        uint8_t maxStep = NUM_STEPS;

        if (forward_)
        {
            currStep_ = (currStep_+1 >= maxStep) ?
                0 :
                (currStep_ + 1);
        }
        else
        {
            currStep_ = (currStep_ == 0) ?
                maxStep - 1 :
                (currStep_ - 1);
        }

        // Set pins according to current step's config
        setPins();
    }

    void BipolarStepperMotor::setPins()
    {
        uint8_t i = 0;
        for (i = 0; i < NUM_PINS; i++)
        {
            Level level = L_LOW;
            if (mode_ == FULL_STEP) level = FullStepDriveTable[currStep_][i];
            if (mode_ == HALF_STEP) level = HalfStepDriveTable[currStep_][i];

            pPins_[i]->set(level);
        }
    }
}