#ifndef ITHERMISTOR_HPP
#define ITHERMISTOR_HPP

namespace Thermistor{

    class IThermistor{
        public:
            virtual void update();
            virtual float getTemperature() = 0;
    };
}


#endif