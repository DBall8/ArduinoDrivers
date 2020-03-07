#ifndef TMP36
#define TMP36

#include "thermistor/IThermistor.hpp"

namespace ThermistorSpace{

    class Tmp36: public IThermistor{
        public:
            Tmp36(int pin, Filter* pFilter);

            float getTemperature() override;
    };
}
#endif