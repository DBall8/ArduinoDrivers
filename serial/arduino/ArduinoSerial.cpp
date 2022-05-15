#include "ArduinoSerial.hpp"
#include <Arduino.h>

namespace SerialComm
{
    ArduinoSerial::ArduinoSerial(int baudRate):
        baudRate_(baudRate)
    {
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
}
