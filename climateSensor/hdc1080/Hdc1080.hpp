#ifndef HDC_1080_HPP
#define HDC_1080_HPP

#include "drivers/climateSensor/IClimateSensor.hpp"
#include "drivers/i2c/II2c.hpp"
#include "drivers/timer/TicCounter.hpp"
#include "drivers/timer/SoftwareTimer.hpp"

namespace ClimateSensor
{
    class Hdc1080ClimateSensor : public IClimateSensor
    {
        public:
            Hdc1080ClimateSensor(I2c::II2c* pI2c, Tic::TicCounter* pTicCounter);
            ~Hdc1080ClimateSensor();

            bool initialize() override;
            bool update() override;
            float getTemperatureCelsius() override;
            float getTemperatureFahrenheit() override;
            float getRelativeHumidity() override;

        private:
            I2c::II2c* pI2c_;
            Timer::SoftwareTimer updateIntervalTimer_;

            float tempC_;
            float humidity_;
    };
}
#endif