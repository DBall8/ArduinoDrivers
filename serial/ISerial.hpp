#ifndef ISERIAL_HPP
#define ISERIAL_HPP

#include <stdint.h>
#include "drivers/timer/SoftwareTimer.hpp"

namespace SerialComm
{
    enum BaudRate: uint8_t
    {
        BAUD_9600 = 0,
        BAUD_115200,
        NUM_BAUD_RATES
    };

    class ISerial
    {
        public:
            virtual void initialize(){};
            virtual bool isDataAvailable() { return false; }
            virtual void write(const uint8_t* buff, uint16_t numBytes) = 0;
            virtual uint16_t read(uint8_t* buff, uint16_t numBytes) = 0;
            virtual void flush(){}
            virtual void flushRx(){}
            virtual void flushTx(){}

            void setTimeoutTimer(Timer::SoftwareTimer* pTimer)
            {
                pTimeoutTimer_ = pTimer;
            }

            virtual void write(const char* buff, uint16_t numBytes)
            {
                write((uint8_t*)buff, numBytes);
            }

            virtual uint16_t read(char* buff, uint16_t numBytes)
            {
                return read((uint8_t*) buff, numBytes);
            }

            /**
             * Read data until a newline character is reached
             * @param   buff        Buffer for received data to be copied to
             * @param   numBytes    Maximum number of received data to copy to buff
             * @return  Number of bytes copied to buff
             */
            virtual uint16_t readLine(uint8_t* buff, uint16_t maxBytes, uint16_t timeoutMs)
            {
                if ((pTimeoutTimer_ != nullptr) &&
                    timeoutMs != 0)
                {
                    pTimeoutTimer_->setPeriodMs(timeoutMs);
                    pTimeoutTimer_->enable();
                }

                uint16_t bytesRead = 0;
                while (bytesRead < maxBytes)
                {
                    if ((pTimeoutTimer_ != nullptr) &&
                        (timeoutMs > 0) &&
                        pTimeoutTimer_->hasOneShotPassed())
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
                }
                return bytesRead;
            }

        protected:
            Timer::SoftwareTimer* pTimeoutTimer_ = nullptr;
    };
}

#endif