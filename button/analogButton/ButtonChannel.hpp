#ifndef BUTTON_CHANNEL_HPP
#define BUTTON_CHANNEL_HPP

#include "AnalogButton.hpp"
#include <stdint.h>

#include "drivers/adc/IAdc.hpp"

namespace Button
{
    class ButtonChannel
    {
        public:
            ButtonChannel(Adc::IAdc* pAdc, AnalogButton** buttonArray, uint8_t numButtons);
            ~ButtonChannel();

            void update();
            AnalogButton* getButton(uint8_t index);

        private:
            AnalogButton** buttonArray_;
            uint8_t numButtons_;

            Adc::IAdc* pAdc_;
    };
}

#endif