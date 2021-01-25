#ifndef IINTERRUPT_HPP
#define IINTERRUPT_HPP

namespace Interrupt
{
    class IInterrupt
    {
        public:
            virtual bool areInterruptsEnabled() = 0;
            virtual void enableInterrupts() = 0;
            virtual void disableInterrupts() = 0;

            /**
             * Pause interrupts
             * This will disable all interrupts, and keep track of if they had been enabled before.
             * Then, when the next resumeInterrupts call is called, interrupts will be enabled only
             * if they had been enabled before
             */
            virtual void pauseInterrupts()
            {
                if (areInterruptsEnabled())
                {
                    areInterruptsPaused_ = true;
                }
                disableInterrupts();
            }

            /**
             * Enables interrupts, but only if a previous pauseInterrupts call had disabled interrupts earlier.
             */
            virtual void resumeInterrupts()
            {
                if (areInterruptsPaused_)
                {
                    areInterruptsPaused_ = false;
                    enableInterrupts();
                }
            }

        private:
            bool areInterruptsPaused_ = false;;
    };
}

#endif