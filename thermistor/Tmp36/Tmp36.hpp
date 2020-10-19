#ifndef TMP36
#define TMP36

#include "drivers/thermistor/IThermistor.hpp"
#include "drivers/adc/IAdc.hpp"
#include "utilities/filter/Filter.hpp"

namespace Thermistor{

    class Tmp36: public IThermistor
    {
        public:
            Tmp36(Adc::IAdc* pAnalogPin, Filter::IFilter* pFilter);

            float getTemperature() override;

        private:
            Adc::IAdc* pAnalogPin_;
            Filter::IFilter* pFilter_;
    };
}
#endif