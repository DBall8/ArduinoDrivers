#include "Tmp36.hpp"
#include "utilities/Conversions.hpp"

using namespace adc;
using namespace filter;

const static float V_TO_C_OFFSET = -0.5f;
const static float V_TO_C_MULTIPLIER = 100.0f;

namespace ThermistorSpace{

    Tmp36::Tmp36(IAdc* pAnalogPin, Filter* pFilter):
        pAnalogPin_(pAnalogPin),
        pFilter_(pFilter)
    {}

    float Tmp36::getTemperature(){
        bool success;
        uint16_t analogReading = pAnalogPin_->read(&success);

        if (!success) return 0;

        pFilter_->addSample((int)analogReading);

        float tempV = convertAdcToVoltage(pFilter_->getValueInt());

        return (tempV + V_TO_C_OFFSET) * V_TO_C_MULTIPLIER;
    }
}