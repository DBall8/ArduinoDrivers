#ifndef ATMEGA328_PWM_HPP
#define ATMEGA328_PWM_HPP

#include "drivers/pwm/IPwm.hpp"
#include "drivers/timer/ATmega328/ATmega328Timer.hpp"
#include "drivers/dio/atmega328/Atmega328Dio.hpp"
#include "drivers/timer/ATmega328/ATmega328Timer.hpp"

namespace Pwm
{
    enum class PwmMode : uint8_t
    {
        FAST,
        PHASE_CORRECT
    };

    enum class PwmResolution : uint16_t
    {
        RES_8_BIT = 0x00FF,
        RES_9_BIT = 0x01FF,
        RES_10_BIT = 0x03FF,
    };

    enum PwmChannel : uint8_t
    {
        CHANNEL_A = 0,
        CHANNEL_B = 1
    };

    struct PwmMapping
    {
        Dio::Port port;
        uint8_t pin;
        Timer::Timer timer;
        PwmChannel channel;
    };

    class Atmega328Pwm : public IPwm
    {
        public:
            Atmega328Pwm(Dio::Port port,
                         uint8_t pin,
                         uint16_t initialDutyCycle = 0,
                         PwmMode mode = PwmMode::FAST,
                         PwmResolution resolution = PwmResolution::RES_10_BIT);
            ~Atmega328Pwm();

            void enable() override;
            void disable() override;

            void setDutyCycle(float percentDuty) override;

        private:
            const PwmMapping* pPwmInfo_;
            PwmResolution resolution_;
    };
}

#endif