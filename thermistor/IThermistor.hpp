#ifndef ITHERMISTOR_HPP
#define ITHERMISTOR_HPP

namespace ThermistorSpace{

    class IThermistor{
        public:
            virtual void update();
            virtual float getTemperature() = 0;
    };
}


#endif