#include "ATmega328Timer.hpp"
#include "drivers/assert/Assert.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

namespace Timer{

    static volatile uint8_t* CONTORL_REG_A[] =
    {
        [TIMER_0] = &TCCR0A,
        [TIMER_1] = &TCCR1A,
        [TIMER_2] = &TCCR2A
    };

    static volatile uint8_t* CONTORL_REG_B[] =
    {
        [TIMER_0] = &TCCR0B,
        [TIMER_1] = &TCCR1B,
        [TIMER_2] = &TCCR2B
    };

    static volatile uint8_t* TIM_INTERRUPT_REG[] =
    {
        [TIMER_0] = &TIMSK0,
        [TIMER_1] = &TIMSK1,
        [TIMER_2] = &TIMSK2
    };

    const static uint8_t CTC_VALUE = (0x01 << 1);
    const static uint8_t CTC_VALUE_TIM1 = (0x01 << 3);
    const static uint8_t COMP_A_INTERRUPT_VAL = (0x01 << 1);

    const static uint8_t PRESCALE_VALUE[] = 
    {
        [PRESCALE_OFF] = 0,
        [PRESCALE_1] = 1,
        [PRESCALE_8] = 2,
        [PRESCALE_32] = 0,
        [PRESCALE_64] = 3,
        [PRESCALE_128] = 0,
        [PRESCALE_256] = 4,
        [PRESCALE_1024] = 5,
        [EXTERNAL_FALLING] = 6,
        [EXTERNAL_RISING] = 7,
    };

    const static uint8_t PRESCALE_VALUE_TIM2[] = 
    {
        [PRESCALE_OFF] = 0,
        [PRESCALE_1] = 1,
        [PRESCALE_8] = 2,
        [PRESCALE_32] = 3,
        [PRESCALE_64] = 4,
        [PRESCALE_128] = 5,
        [PRESCALE_256] = 6,
        [PRESCALE_1024] = 7,
        [EXTERNAL_FALLING] = 0,
        [EXTERNAL_RISING] = 0,
    };

    Atmega328Timer::Atmega328Timer(Timer timer,
                                   TimerMode mode,
                                   TimerPrescaler prescaler,
                                   uint16_t top,
                                   void (*interrupt)(void)):
        timer_(timer),
        mode_(mode),
        prescaler_(prescaler),
        top_(top)
    {
        switch (timer_)
        {
            case TIMER_0:
            {
                // Must be 8 bit
                assertCustom(top_ < 0x100);

                // Must be a supported prescaler
                assertCustom(PRESCALE_VALUE[prescaler_] > 0);
                break;
            }

            case TIMER_1:
            {
                // Must be a supported prescaler
                assertCustom(PRESCALE_VALUE[prescaler_] > 0);
                break;
            }

            default:
            case TIMER_2:
            {
                // Must be 8 bit
                assertCustom(top_ < 0x100);

                // Must be a supported prescaler
                assertCustom(PRESCALE_VALUE_TIM2[prescaler_] > 0);
                break;
            }
        }
        setCompAInterrupt(timer, interrupt);
    }

    void Atmega328Timer::initialize()
    {
        // Clear control registers
        *CONTORL_REG_A[timer_] = 0;
        *CONTORL_REG_B[timer_] = 0;

        switch (timer_)
        {
            case TIMER_0:
            {
                // Set mode
                if (mode_ == CTC){
                    *CONTORL_REG_A[timer_] |= CTC_VALUE;
                }

                // Set prescaler
                *CONTORL_REG_B[timer_] |= PRESCALE_VALUE[prescaler_];

                // Set the compare A top value (8 bit)
                OCR0A = (uint8_t)top_;
                break;
            }

            case TIMER_1:
            {
                // Set mode
                if (mode_ == CTC){
                    *CONTORL_REG_B[timer_] |= CTC_VALUE_TIM1;
                }

                // Set prescaler
                *CONTORL_REG_B[timer_] |= PRESCALE_VALUE[prescaler_];

                // Set the compare A top value (16 bit)
                OCR1A = top_;
            }

            default:
            case TIMER_2:
            {
                // Set mode
                if (mode_ == CTC){
                    *CONTORL_REG_A[timer_] |= CTC_VALUE;
                }

                // Set prescaler
                *CONTORL_REG_B[timer_] |= PRESCALE_VALUE_TIM2[prescaler_];

                // Set the compare A top value (8 bit)
                OCR2A = (uint8_t)top_;
                break;
            }
        }

        // Enable timer interrupt
        enableCompAInterrupt(timer_);
    }

    void Atmega328Timer::setCompAInterrupt(Timer timer, void (*interrupt)(void)){

        switch (timer){
            case TIMER_0:
            {
                Timer0CompareAInterrupt = interrupt;
                break;
            }
            case TIMER_1:
            {
                Timer1CompareAInterrupt = interrupt;
                break;
            }
            case TIMER_2:
            {
                Timer2CompareAInterrupt = interrupt;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void Atmega328Timer::enableCompAInterrupt(Timer timer)
    {
        *TIM_INTERRUPT_REG[timer] |= COMP_A_INTERRUPT_VAL;
    }

    void Atmega328Timer::disableCompAInterrupt(Timer timer){
        *TIM_INTERRUPT_REG[timer] &= ~COMP_A_INTERRUPT_VAL;
    }

    ISR(TIMER0_COMPA_vect){
        Timer0CompareAInterrupt();
    }

    ISR(TIMER1_COMPA_vect){
        Timer1CompareAInterrupt();
    }

    ISR(TIMER2_COMPA_vect){
        Timer2CompareAInterrupt();
    }
}