#ifndef DIGITAL_BUTTON_HPP
#define DIGITAL_BUTTON_HPP

#include "drivers/button/IButton.hpp"
#include "drivers/dio/IDio.hpp"
#include "drivers/timer/SoftwareTimer.hpp"

namespace Button
{
    class DigitalButton : public IButton
    {
        public:
            DigitalButton(Dio::IDio* pDio, Timer::SoftwareTimer* pDebounceTimer = nullptr);
            ~DigitalButton();

            void update() override;
            ButtonState getState() override;
            ButtonTransition getTransition() override;

        private:
            Dio::IDio* pDio_;
            Timer::SoftwareTimer* pDebounceTimer_;
            ButtonState currentState_;
            ButtonState prevState_;
    };
}

#endif