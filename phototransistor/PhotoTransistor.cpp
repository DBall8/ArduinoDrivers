#include "PhotoTransistor.hpp"
#include <Arduino.h>

namespace photoTransistor{

    const float MAX_ADC = 1023.0f;

    PhotoTransistor::PhotoTransistor(int pin, Filter* pFilter):
        pin_(pin),
        pFilter_(pFilter)
    {}

    void PhotoTransistor::update(){
        pFilter_->addSample(analogRead(pin_));
    }

    float PhotoTransistor::getLightPercent(){
        return (pFilter_->getValueInt() / MAX_ADC) * 100.0f;
    }
}