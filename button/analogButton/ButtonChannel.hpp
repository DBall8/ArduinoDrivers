#ifndef BUTTON_CHANNEL_HPP
#define BUTTON_CHANNEL_HPP

#include "AnalogButton.hpp"
#include <stdint.h>

namespace button
{
    class ButtonChannel
    {
        public:
            ButtonChannel(uint8_t adcChannel, AnalogButton** buttonArray, uint8_t numButtons);
            ~ButtonChannel();

            void update();
            AnalogButton* getButton(uint8_t index);

        private:
            AnalogButton** buttonArray_;
            uint8_t numButtons_;

            uint8_t adcChannel_;
    };
}

#endif