#include "ButtonChannel.hpp"

#include "drivers/assert/Assert.hpp"
#include <Arduino.h>

namespace button
{
    ButtonChannel::ButtonChannel(uint8_t adcChannel, AnalogButton** buttonArray, uint8_t numButtons):
        buttonArray_(buttonArray),
        numButtons_(numButtons),
        adcChannel_(adcChannel)
    {
    }

    ButtonChannel::~ButtonChannel(){}

    void ButtonChannel::update()
    {
        uint16_t reading = analogRead(adcChannel_);
        for (uint8_t i=0; i<numButtons_; i++)
        {
            buttonArray_[i]->update(reading);
        }
    }

    AnalogButton* ButtonChannel::getButton(uint8_t index)
    {
        assert(index < numButtons_);
        
        return buttonArray_[index];
    }

}