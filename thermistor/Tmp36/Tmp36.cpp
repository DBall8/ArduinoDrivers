#include "Tmp36.hpp"
#include "utilities/Conversions.hpp"

const static float V_TO_C_OFFSET = -0.5f;
const static float V_TO_C_MULTIPLIER = 100.0f;

namespace ThermistorSpace{

    Tmp36::Tmp36(int pin, Filter* pFilter):
        IThermistor(pin, pFilter)
    {}

    float Tmp36::getTemperature(){
        float tempV = convertAdcToVoltage(pFilter_->getValueInt());

        return (tempV + V_TO_C_OFFSET) * V_TO_C_MULTIPLIER;
    }
}