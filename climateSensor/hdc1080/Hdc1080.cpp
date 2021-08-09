#include "Hdc1080.hpp"
#include "drivers/timer/Delay.hpp"
#include "utilities/Conversions.hpp"

using namespace I2c;
using namespace Timer;
using namespace Tic;

const static uint8_t SLAVE_ID = 0x40;  // 7 bit slave address

const static uint8_t TEMP_REG = 0x00;   // Register for temperature value
const static uint8_t HUMID_REG = 0x01;  // Register for humidity value
const static uint8_t CONFIG_REG = 0x02; // Register for configuration

const static uint8_t HEATER_BIT = 13;   // Heater bit, 0 = off, 1 = on
const static uint8_t MODE_BIT = 12;     // Mode selection bit, 1 = read both together, 0 = read humid/temp separately
const static uint8_t BATTERY_STATUS_BIT = 11; // Read only, 1 = < 2.8 V, 0 = > 2.8 V
const static uint8_t TEMP_RES_BIT = 10; // Temperature resolution, 0 = 14 bit, 1 = 11 bit
const static uint8_t HUMID_RES_BIT = 8; // Humidity resolution, 0 = 14 bit, 1 = 11 bit, 2 = 8 bit

const static int16_t MAX_TEMP_C = 125;
const static int16_t MIN_TEMP_C = -40;

const static int16_t MAX_HUMID = 100;
const static int16_t MIN_HUMID = 0;

namespace ClimateSensor
{
    Hdc1080ClimateSensor::Hdc1080ClimateSensor(II2c* pI2c,
                                               TicCounter* pTicCounter):
        pI2c_(pI2c),
        updateIntervalTimer_(pTicCounter->msecondsToTics(7), pTicCounter),
        tempC_(0),
        humidity_(0)
    {

    }

    Hdc1080ClimateSensor::~Hdc1080ClimateSensor()
    {
    }

    bool Hdc1080ClimateSensor::initialize()
    {
        // Configuration
        const static uint16_t configuration = (1 << MODE_BIT);
        static uint8_t configTx[] = {CONFIG_REG,                    // Write to config register
                                    (uint8_t)(configuration >> 8),  // Config MSB
                                    (uint8_t)configuration};        // Config LSB
        const static uint8_t configLen = sizeof(configTx);

        // Requires at most 15 ms powerup
        DELAY(15);

        pI2c_->enable();

        // Return the success of the transmission
        return pI2c_->transmit(SLAVE_ID, configTx, configLen);
    }

    bool Hdc1080ClimateSensor::update()
    {
        // Create buffer for reading values
        const static uint8_t NUM_CLIMATE_BYTES = 4; // 2 bytes for temperature, 2 of humidity
        uint8_t readBuffer[NUM_CLIMATE_BYTES];

        // Transmit temperature register to start measurement
        if (!pI2c_->transmit(SLAVE_ID, TEMP_REG))
        {
            // Failed to trigger measurement
            return false;
        }

        // Wait at least 15 ms for measurement to be taken
        DELAY(20);

        // Retrieve the new measurements
        if (!pI2c_->receive(SLAVE_ID, readBuffer, NUM_CLIMATE_BYTES))
        {
            // Failed to read measurements
            return false;
        }

        // Combine each two bytes to form each reading.

        // Temperature reading
        uint16_t tempReading = readBuffer[0] << 8;
        tempReading += readBuffer[1];

        // Humidity reading
        uint16_t humidReading = readBuffer[2] << 8;
        humidReading += readBuffer[3];

        // Readings are linear, where 0 is the min value and uint16_max is the max value
        // Therefore, we need to convert to proper values

        float tempFraction = tempReading / (float)UINT16_MAX;   // Position in linear range
        tempC_ = ((MAX_TEMP_C - MIN_TEMP_C) * tempFraction) + MIN_TEMP_C;

        float humidFraction = humidReading / (float)UINT16_MAX;
        humidity_ = ((MAX_HUMID - MIN_HUMID) * humidFraction) + MIN_HUMID;

        return true;
    }

    float Hdc1080ClimateSensor::getTemperatureCelsius()
    {
        return tempC_;
    }

    float Hdc1080ClimateSensor::getTemperatureFahrenheit()
    {
        return degreesCToF(tempC_);
    }

    float Hdc1080ClimateSensor::getRelativeHumidity()
    {
        return humidity_;
    }
}