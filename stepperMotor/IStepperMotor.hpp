#ifndef ISTEPPER_MOTOR_HPP
#define ISTEPPER_MOTOR_HPP

#include <stdint.h>

namespace StepperMotor
{
    enum StepperMode : uint8_t
    {
        FULL_STEP = 0,
        HALF_STEP,
        MICRO_STEP,
        STEPPER_MODE_MAX
    };

    class IStepperMotor
    {
        public:
            virtual void setMode(StepperMode mode) = 0;
            virtual StepperMode getMode() = 0;
            virtual void stop() = 0;
            virtual void run(float speed) = 0;
            virtual void powerDown() = 0;
    };
}

#endif