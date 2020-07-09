#ifndef DIGITAL_BUTTON_HPP
#define DIGITAL_BUTTON_HPP

#include "drivers/button/IButton.hpp"
#include "drivers/dio/IDio.hpp"

namespace button
{
    class DigitalButton : public IButton
    {
        public:
            DigitalButton(dio::IDio* pDio);
            ~DigitalButton();

            void update() override;
            ButtonState getState() override;
            ButtonTransition getTransition() override;

        private:
            dio::IDio* pDio_;
            ButtonState currentState_;
            ButtonState prevState_;
    };
}

#endif