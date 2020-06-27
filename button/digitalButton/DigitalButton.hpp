#ifndef DIGITAL_BUTTON_HPP
#define DIGITAL_BUTTON_HPP

#include "drivers/button/IButton.hpp"

namespace button
{
    class DigitalButton : public IButton
    {
        public:
            DigitalButton(uint8_t digitalPin);
            ~DigitalButton();

            void update() override;
            ButtonState getState() override;
            ButtonTransition getTransition() override;

        private:
            uint8_t digitalPin_;
            ButtonState currentState_;
            ButtonState prevState_;
    };
}

#endif