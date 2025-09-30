#ifndef BIPOLAR_STEPPER_MOTOR_HPP
#define BIPOLAR_STEPPER_MOTOR_HPP

#include "drivers/stepperMotor/IStepperMotor.hpp"
#include "drivers/dio/IDio.hpp"
#include "drivers/timer/ITimer.hpp"

namespace StepperMotor
{
    class BipolarStepperMotor : public IStepperMotor
    {
        public:
            const static int NUM_PINS = 4;

            BipolarStepperMotor(
                Timer::ITimer* pTimer,
                Dio::IDio* pPinA,
                Dio::IDio* pPinB,
                Dio::IDio* pPinC,
                Dio::IDio* pPinD);

            void setMode(StepperMode mode) override;
            StepperMode getMode() override;
            void stop() override;
            void run(float speed) override;
            void powerDown() override;

            void interruptHandler();

        private:
            Timer::ITimer* pTimer_;
            Dio::IDio* pPins_[NUM_PINS] = {nullptr};

            StepperMode mode_;
            uint8_t currStep_;
            uint16_t updateCount_;
            bool forward_;
            volatile uint16_t count_;

            void setPins();
    };
}

#endif