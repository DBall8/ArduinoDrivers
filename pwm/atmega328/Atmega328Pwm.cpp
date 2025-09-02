#include "Atmega328Pwm.hpp"
#include "drivers/assert/Assert.hpp"
#include <avr/io.h>

using namespace Dio;
using namespace Timer;

namespace Pwm
{
    const static PwmMapping PWM_MAPPINGS[] = 
    {
        {.port = Port::D, 6, Timer::TIMER_0, PwmChannel::CHANNEL_A},
        {.port = Port::D, 5, Timer::TIMER_0, PwmChannel::CHANNEL_B},
        {.port = Port::B, 1, Timer::TIMER_1, PwmChannel::CHANNEL_A},
        {.port = Port::B, 2, Timer::TIMER_1, PwmChannel::CHANNEL_B},
        {.port = Port::B, 3, Timer::TIMER_2, PwmChannel::CHANNEL_A},
        {.port = Port::D, 3, Timer::TIMER_2, PwmChannel::CHANNEL_B}
    };
    const static uint8_t NUM_PWM_MAPPINGS = sizeof(PWM_MAPPINGS)/sizeof(PWM_MAPPINGS[0]);

    Atmega328Pwm::Atmega328Pwm(Port port,
                               uint8_t pin,
                               uint16_t initialDutyCycle,
                               PwmMode mode,
                               PwmResolution resolution):
        pPwmInfo_(nullptr),
        resolution_(resolution),
        dutyCycle_(0),
        enabled_(false)
    {
        // Look at the PWM pins to find the one that matches the inputs
        for (uint8_t i=0; i<NUM_PWM_MAPPINGS; i++)
        {
            PwmMapping mapping = PWM_MAPPINGS[i];
            if ((mapping.port == port) &&
                (mapping.pin == pin))
            {
                // Located the correct PWM mapping
                pPwmInfo_ = &(PWM_MAPPINGS[i]);
            }
        }

        assertCustom(pPwmInfo_ != nullptr, "Invalid PWM pin.");
        assertCustom(mode != PwmMode::PHASE_CORRECT, "Phase correct PWM not yet supported.");

        // Set up the corresponding pin as an output
        Atmega328Dio pwmPin(port, pin, Dio::Mode::OUTPUT, Level::L_LOW, false, false);

        if (pPwmInfo_->timer == Timer::TIMER_0)
        {
            uint8_t regA = TCCR0A;
            uint8_t regB = 0;
            
            regB = (1 << CS00); // No prescaler for maximum frequency

            if (resolution_ == PwmResolution::RES_8_BIT)
            {
                regA |=  (1 << WGM01) | (1 << WGM00);
            }
            else
            {
                assertCustom(false, "Only 8 bit PWM supported for this pin.");
            }

            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                regA |= (1 << COM0A1);  // Non-inverted channel A 
            }
            else
            {
                regA |= (1 << COM0B1);   // Non-inverted channel B 
            }

            TCCR0A = regA;
            TCCR0B = regB;
        }
        else if (pPwmInfo_->timer == Timer::TIMER_1)
        {
            uint8_t regA = TCCR1A;
            uint8_t regB = 0;
            
            regB = (1 << CS10) | // No prescaler for maximum frequency
                   (1 << WGM12); // Fast PWM resolution first bit

            if (resolution_ == PwmResolution::RES_8_BIT)
            {
                regA |=  (1 << WGM10);
            }
            else if (resolution_ == PwmResolution::RES_9_BIT)
            {
                regA |= (1 << WGM11);
            }
            else
            {
                regA |= (1 << WGM11) | (1 << WGM10);
            }

            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                regA |= (1 << COM1A1);  // Non-inverted channel A 
                regA &= ~(1 << COM1A0);
            }
            else
            {
                regA |= (1 << COM1B1);   // Non-inverted channel B 
                regA &= ~(1 << COM1B0);
            }

            TCCR1A = regA;
            TCCR1B = regB;
        }
        else if (pPwmInfo_->timer == Timer::TIMER_2)
        {
            uint8_t regA = TCCR2A;
            uint8_t regB = 0;
            
            regB = (1 << CS20); // No prescaler for maximum frequency

            if (resolution_ == PwmResolution::RES_8_BIT)
            {
                regA |=  (1 << WGM21) | (1 << WGM20);
            }
            else
            {
                assertCustom(false, "Only 8 bit PWM supported for this pin.");
            }

            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                regA |= (1 << COM2A1);  // Non-inverted channel A 
            }
            else
            {
                regA |= (1 << COM2B1);   // Non-inverted channel B 
            }

            TCCR2A = regA;
            TCCR2B = regB;
        }

        setDutyCycle(initialDutyCycle);
    }

    Atmega328Pwm::~Atmega328Pwm()
    {}

    void Atmega328Pwm::enable()
    {
        enabled_ = true;
        setDutyBinary(dutyCycle_);
    }

    void Atmega328Pwm::disable()
    {
        uint16_t currentDutyCycle = dutyCycle_;
        setDutyBinary(0);
        dutyCycle_ = currentDutyCycle;

        enabled_ = false;
    }

    void Atmega328Pwm::setDutyCycle(float percentDuty)
    {
        uint16_t dutyCycle = (uint16_t)resolution_ * percentDuty / 100;
        dutyCycle_ = dutyCycle;
        if (!enabled_) return;

        setDutyBinary(dutyCycle);
    }

    uint16_t Atmega328Pwm::getResolution()
    {
        return (uint16_t)resolution_;
    }

    void Atmega328Pwm::setDutyBinary(uint16_t dutyBinary)
    {
        dutyCycle_ = dutyBinary;
        if (!enabled_) return;

        if (pPwmInfo_->timer == Timer::TIMER_0)
        {
            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                OCR0A = (uint8_t)dutyCycle_;
            }
            else if (pPwmInfo_->channel == PwmChannel::CHANNEL_B)
            {
                OCR0B = (uint8_t)dutyCycle_;
            }
        }
        else if (pPwmInfo_->timer == Timer::TIMER_1)
        {
            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                OCR1AH = (uint8_t)(dutyCycle_ >> 8);
                OCR1AL = (uint8_t)(dutyCycle_);
            }
            else if (pPwmInfo_->channel == PwmChannel::CHANNEL_B)
            {
                OCR1BH = (uint8_t)(dutyCycle_ >> 8);
                OCR1BL = (uint8_t)(dutyCycle_);
            }
            
        }
        else if (pPwmInfo_->timer == Timer::TIMER_2)
        {
            if (pPwmInfo_->channel == PwmChannel::CHANNEL_A)
            {
                OCR2A = (uint8_t)dutyCycle_;
            }
            else if (pPwmInfo_->channel == PwmChannel::CHANNEL_B)
            {
                OCR2B = (uint8_t)dutyCycle_;
            }
        }
    }
}