#ifndef TMP36
#define TMP36

#include "drivers/thermistor/IThermistor.hpp"
#include "drivers/adc/IAdc.hpp"
#include "utilities/filter/Filter.hpp"

namespace ThermistorSpace{

    class Tmp36: public IThermistor{
        public:
            Tmp36(adc::IAdc* pAnalogPin, filter::Filter* pFilter);

            float getTemperature() override;

        private:
            adc::IAdc* pAnalogPin_;
            filter::Filter* pFilter_;
    };
}
#endif