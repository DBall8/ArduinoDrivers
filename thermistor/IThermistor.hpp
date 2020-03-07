#ifndef ITHERMISTOR_HPP
#define ITHERMISTOR_HPP

#include "utilities/filter/Filter.hpp"

using namespace FilterSpace;

namespace ThermistorSpace{

    class IThermistor{
        public:
            int pin_;
            Filter* pFilter_;

            IThermistor(int pin, Filter* pFilter);

            virtual void update();
            virtual float getTemperature() = 0;
    };
}


#endif