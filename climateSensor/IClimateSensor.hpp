#ifndef I_CLIMATE_SENSOR_HPP
#define I_CLIMATE_SENSOR_HPP

namespace ClimateSensor
{
    class IClimateSensor
    {
        public:
            virtual bool initialize(){return true;}
            virtual bool update(){return false;}
            virtual float getTemperatureCelsius() = 0;
            virtual float getTemperatureFahrenheit() = 0;
            virtual float getRelativeHumidity() = 0;
    };
}

#endif