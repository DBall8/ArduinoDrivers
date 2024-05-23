#include "ArduinoSerial.hpp"
#include <Arduino.h>

namespace SerialComm
{
    ArduinoSerial::ArduinoSerial(int baudRate, Timer::SoftwareTimer* pTimeoutTimer):
        baudRate_(baudRate)
    {
        setTimeoutTimer(pTimeoutTimer);
    }
    ArduinoSerial::~ArduinoSerial(){}

    void ArduinoSerial::initialize()
    {
        Serial.begin(baudRate_);
    }

    bool ArduinoSerial::isDataAvailable()
    {
        return Serial.available();
    }

    void ArduinoSerial::write(const uint8_t* buff, uint16_t numBytes)
    {
        Serial.write(buff, numBytes);
    }

    uint16_t ArduinoSerial::read(uint8_t* buff, uint16_t numBytes)
    {
        return Serial.readBytes(buff, numBytes);
    }
    
    void ArduinoSerial::flush()
    {
        Serial.flush();
    }

    uint16_t ArduinoSerial::readLine(uint8_t* buff, uint16_t maxBytes, uint16_t timeoutMs)
    {
        uint32_t timeStartMs = millis();

        uint16_t bytesRead = 0;
        while (bytesRead < maxBytes)
        {
            uint32_t currTimeMs = millis();
            if ((timeoutMs > 0) &&
                ((currTimeMs - timeStartMs) > timeoutMs))
            {
                return bytesRead;
            }

            if (read(&(buff[bytesRead]), 1))
            {
                bytesRead++;

                if (buff[bytesRead-1] == '\n')
                {
                    buff[bytesRead-1] = '\0';
                    return bytesRead;
                }
            }

#ifdef ESP8266
            ESP.wdtFeed();
#endif
        }
        return bytesRead;
    }
}
