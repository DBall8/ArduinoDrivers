#include "ButtonChannel.hpp"
#include "drivers/assert/Assert.hpp"

using namespace Adc;

namespace Button
{
    ButtonChannel::ButtonChannel(IAdc* pAdc, AnalogButton** buttonArray, uint8_t numButtons):
        buttonArray_(buttonArray),
        numButtons_(numButtons),
        pAdc_(pAdc)
    {
    }

    ButtonChannel::~ButtonChannel(){}

    void ButtonChannel::update()
    {
        uint16_t reading = pAdc_->read();
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