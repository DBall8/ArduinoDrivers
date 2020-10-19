#include "PhotoTransistor.hpp"

using namespace Adc;
using namespace Filter;

const float MAX_ADC = 1023.0f;

PhotoTransistor::PhotoTransistor(IAdc* pAdc, IFilter* pFilter):
    pAdc_(pAdc),
    pFilter_(pFilter)
{
    pAdc_->enable();
}

void PhotoTransistor::update(){
    bool success;

    uint16_t reading = pAdc_->read(&success);
    if (!success) return;

    pFilter_->addSample((int)reading);
}

float PhotoTransistor::getLightPercent(){
    return (pFilter_->getValueInt() / MAX_ADC) * 100.0f;
}