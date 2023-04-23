#ifndef ATMEGA328_TIMER_HPP
#define ATMEGA328_TIMER_HPP

#include "drivers/timer/ITimer.hpp"

namespace Timer
{
    enum Timer{
        TIMER_0 = 0,
        TIMER_1,
        TIMER_2
    };
    
    enum TimerMode{
        NORMAL,             // Normal counting
        CTC,                // Clear Timer on Capture
    };

    static void (*Timer0CompareAInterrupt)(void) = nullptr;
    static void (*Timer1CompareAInterrupt)(void) = nullptr;
    static void (*Timer2CompareAInterrupt)(void) = nullptr;

    class Atmega328Timer : public ITimer
    {
        public:
            /**
             * Frequency_target = F_CPU / (Prescaler * (Top+1))
             *
             * @param   timer       The timer to use, Timer1 is 16bit, Timers0 and 2 are 8-bit
             * @param   mode        Normal counting or clear on top match (reset to 0 at top or wait for rollover)
             * @param   prescale    Used in frequency target calculation
             * @param   top         Trigger interrupt when the timer counts to this value
             * @param   interrupt   Interrupt to call on compare match
             */
            Atmega328Timer(Timer timer, TimerMode mode, TimerPrescaler prescaler, uint16_t top, void (*interrupt)(void));

            /**
             * Start up timer, must be done after static initialization
             */
            void initialize();

            void setPeriodTics(uint16_t tics) override;
            uint16_t microSecondsToTics(uint32_t microseconds) override;
            void reset() override;

            void enable() override;
            void disable() override;
            void setInterrupt(void (*interrupt)(void)) override;

        private:
            Timer timer_;
            TimerMode mode_;
            TimerPrescaler prescaler_;
            uint16_t top_;
            bool enabled_;

            void setTop(uint16_t top);
            
            void setCompAInterrupt(Timer timer, void (*interrupt)(void));
            void enableCompAInterrupt(Timer timer);
            void disableCompAInterrupt(Timer timer);
    };
}

#endif