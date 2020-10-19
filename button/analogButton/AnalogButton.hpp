#ifndef ANALOG_BUTTON_HPP
#define ANALOG_BUTTON_HPP

#include "drivers/button/IButton.hpp"

namespace Button
{
    struct ValidRange
    {
        uint16_t analogMin;
        uint16_t analogMax;
    };

    class AnalogButton : public IButton
    {
        public:
            AnalogButton(ValidRange* validRanges, uint8_t numValidRanges);
            ~AnalogButton();

            void update(uint16_t analogReading);
            ButtonState getState() override;
            ButtonTransition getTransition() override;

        private:
            ButtonState state_;
            ButtonTransition transition_;

            uint8_t numValidRanges_;
            ValidRange* validRanges_;
    };
}

#endif