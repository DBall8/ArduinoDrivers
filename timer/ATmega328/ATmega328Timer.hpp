#ifndef ATMEGA328_TIMER_HPP
#define ATMEGA328_TIMER_HPP

namespace ATmega328Timer{

    enum Timer{
        TIMER_0 = 0,
        TIMER_1,
        TIMER_2
    };
    
    enum TimerMode{
        NORMAL, // Normal counting
        CTC     // Clear Timer on Capture
    };

    enum TimerPrescaler{
        PRESCALE_1,
        PRESCALE_8,
        PRESCALE_32,
        PRESCALE_64,
        PRESCALE_128,
        PRESCALE_256,
        PRESCALE_1024
    };
#ifdef ENABLE_TIM0_COMPA
    static void (*Timer0CompareAInterrupt)(void) = nullptr;
#endif
#ifdef ENABLE_TIM1_COMPA
    static void (*Timer1CompareAInterrupt)(void) = nullptr;
#endif
#ifdef ENABLE_TIM2_COMPA
    static void (*Timer2CompareAInterrupt)(void) = nullptr;
#endif

    // TOP = (ClockRate / (prescaler * TargetFrequency)) - 1
    void InitializeTimer(Timer timer, TimerMode mode, TimerPrescaler prescale, int top);

    void SetCompAInterrupt(Timer timer, void (*interrupt)(void));

    void EnableCompAInterrupt(Timer timer);
    void DisableCompAInterrupt(Timer timer);
}

#endif