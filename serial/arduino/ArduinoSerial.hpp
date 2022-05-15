#ifndef ARDUINO_SERIAL_HPP
#define ARDUINO_SERIAL_HPP

#include "drivers/serial/ISerial.hpp"

namespace SerialComm
{
    class ArduinoSerial : public ISerial
    {
        public:
            ArduinoSerial(int baudRate);
            ~ArduinoSerial();

            void initialize() override;
            bool isDataAvailable() override;
            void write(const uint8_t* buff, uint16_t numBytes)  override;
            uint16_t read(uint8_t* buff, uint16_t numBytes)  override;
            void flush() override;

        private:
            int baudRate_;
    };
}

#endif