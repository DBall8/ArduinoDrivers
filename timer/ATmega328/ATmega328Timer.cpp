#include "ATmega328Timer.hpp"
#include <Arduino.h>

namespace ATmega328Timer{

    void InitializeTimer(Timer timer,
                         TimerMode mode,
                         TimerPrescaler prescaler,
                         int top)
    {
        switch (timer)
        {
            case TIMER_0:
            {
                // 8-bit timer control registers
                TCCR0A  = 0;
                TCCR0B  = 0;

                if (mode == CTC){
                    TCCR0A |= (1 << WGM01); // CTC mode
                }

                if (prescaler == PRESCALE_1 ||
                    prescaler == PRESCALE_64 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR0B |= (1 << CS00);
                }

                if (prescaler == PRESCALE_8 ||
                    prescaler == PRESCALE_64)
                {
                    TCCR0B |= (1 << CS01);
                }

                if (prescaler == PRESCALE_256 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR0B |= (1 << CS02);
                }

                // Compare A
                OCR0A = (uint8_t)top;
                break;
            }
            case TIMER_1:
            default:
            {
                // 8-bit timer control registers
                TCCR1A  = 0;
                TCCR1B  = 0;

                if (mode == CTC){
                    TCCR1B |= (1 << WGM12); // CTC mode
                }

                if (prescaler == PRESCALE_1 ||
                    prescaler == PRESCALE_64 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR1B |= (1 << CS10);
                }

                if (prescaler == PRESCALE_8 ||
                    prescaler == PRESCALE_64)
                {
                    TCCR1B |= (1 << CS11);
                }

                if (prescaler == PRESCALE_256 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR1B |= (1 << CS12);
                }

                // Compare A
                OCR1A = top;
            }

            case TIMER_2:
            {
                // 8-bit timer control registers
                TCCR2A  = 0;
                TCCR2B  = 0;

                if (mode == CTC){
                    TCCR2A |= (1 << WGM21); // CTC mode
                }

                if (prescaler == PRESCALE_1 ||
                    prescaler == PRESCALE_32 ||
                    prescaler == PRESCALE_128 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR2B |= (1 << CS20);
                }

                if (prescaler == PRESCALE_8 ||
                    prescaler == PRESCALE_32 ||
                    prescaler == PRESCALE_256 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR2B |= (1 << CS21);
                }

                if (prescaler == PRESCALE_64 ||
                    prescaler == PRESCALE_128 ||
                    prescaler == PRESCALE_256 ||
                    prescaler == PRESCALE_1024)
                {
                    TCCR2B |= (1 << CS22);
                }

                // Compare A
                OCR2A = (uint8_t)top;
                break;
            }
        }
    }

    void SetCompAInterrupt(Timer timer, void (*interrupt)(void)){

        switch (timer){
            case TIMER_0:
            {
#ifdef ENABLE_TIM0_COMPA
                Timer0CompareAInterrupt = interrupt;
#endif
                break;
            }
            case TIMER_1:
            {
#ifdef ENABLE_TIM1_COMPA
                Timer1CompareAInterrupt = interrupt;
#endif
                break;
            }
            case TIMER_2:
            {
#ifdef ENABLE_TIM2_COMPA
                Timer2CompareAInterrupt = interrupt;
#endif
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void EnableCompAInterrupt(Timer timer){
        switch (timer){
            case TIMER_0:
            {
                TIMSK0 |= (1 << OCIE0A);
                break;
            }

            case TIMER_1:
            {
                TIMSK1 |= (1 << OCIE1A);
                break;
            }

            case TIMER_2:
            {
                TIMSK2 |= (1 << OCIE2A); // Untested
                break;
            }
        }
    }

    void DisableCompAInterrupt(Timer timer){
        switch (timer){
            case TIMER_0:
            {
                TIMSK0 &= ~(1 << OCIE0A); // Untested
                break;
            }

            case TIMER_1:
            {
                TIMSK1 &= ~(1 << OCIE1A);
                break;
            }

            case TIMER_2:
            {
                TIMSK2 &= ~(1 << OCIE2A); // Untested
                break;
            }
        }
    }

#ifdef ENABLE_TIM0_COMPA
    ISR(TIMER0_COMPA_vect){
        Timer0CompareAInterrupt();
    }
#endif

#ifdef ENABLE_TIM1_COMPA
    ISR(TIMER1_COMPA_vect){
        Timer1CompareAInterrupt();
    }
#endif

#ifdef ENABLE_TIM2_COMPA
    ISR(TIMER2_COMPA_vect){
        Timer2CompareAInterrupt();
    }
#endif
}