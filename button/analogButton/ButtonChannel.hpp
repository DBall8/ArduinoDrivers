#ifndef BUTTON_CHANNEL_HPP
#define BUTTON_CHANNEL_HPP

#include "AnalogButton.hpp"
#include <stdint.h>

#include "drivers/adc/IAdc.hpp"

namespace button
{
    class ButtonChannel
    {
        public:
            ButtonChannel(adc::IAdc* pAdc, AnalogButton** buttonArray, uint8_t numButtons);
            ~ButtonChannel();

            void update();
            AnalogButton* getButton(uint8_t index);

        private:
            AnalogButton** buttonArray_;
            uint8_t numButtons_;

            adc::IAdc* pAdc_;
    };
}

#endif