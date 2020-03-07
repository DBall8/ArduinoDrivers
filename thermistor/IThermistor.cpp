#include "IThermistor.hpp"
#include <Arduino.h>

namespace ThermistorSpace{

    IThermistor::IThermistor(int pin, Filter* pFilter):
        pin_(pin),
        pFilter_(pFilter)
    {}

    void IThermistor::update(){
        pFilter_->addSample(analogRead(pin_));
    }
}