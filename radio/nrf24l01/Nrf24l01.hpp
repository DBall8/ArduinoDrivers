#ifndef NRF24L01_HPP
#define NRF24L01_HPP

#include "spi/SpiDriver.hpp"

using namespace spi;

namespace radio
{
    enum DataSpeed: uint8_t
    {
        RF_250_KBPS,
        RF_1_MBPS,
        RF_2_MBPS
    };

    enum PaLevel: uint8_t
    {
        PA_LOW = 0,
        PA_MED,
        PA_HIGH,
        PA_MAX
    };

    class Nrf24l01
    {
        public:
            Nrf24l01(uint8_t cePin, SpiDriver* pSpi);
            ~Nrf24l01();

            /**
             * Set up radio with default settings
             */
            void initialize();

            /**
             * Set PA level, higher level has higher range at the cost of higher energy consumption
             */
            void setPaLevel(PaLevel paLevel);

            /**
             * Set data speed, higher speed allows for faster transmissions but is
             * less reliable
             */
            void setDataSpeed(DataSpeed dataSpeed);

            /**
             * Set the RF channel to operate on
             */
            void setChannel(uint8_t channel);

            /**
             * Setup how the transmitter resends transmissions if it fails to be ACKed
             *
             * @param   numRetries                  number of retries before transmission fails
             * @param   retransmitDelayMultiplier   This times 250 microseconds wait time between retries
             */
            void setupRetries(uint8_t numRetries, uint8_t retransmitDelayMultiplier);

            /**
             * Set up radio to start transmitting
             * @param   address  address to send to
             */
            void startTransmitting(char* address);

            /**
             * Send data, MUST have called startTransmitting first
             * @param   buffer      data to send
             * @param   numBytes    number of bytes in transmit buffer
             */
            bool transmit(uint8_t* buffer, uint8_t numBytes);

            /**
             * Empty transmit and receive buffers
             */
            void flush();

            /**
             * Stop sending or receiving, go into low power mode
             */
            void stop();

            bool startListening(uint8_t pipeIndex, char* address);

            bool isAvailable();

            bool read(uint8_t* buff, uint8_t numBytes);

            void stopListening();

        private:
            uint8_t cePin_;
            SpiDriver* pSpi_;

            /**
             * Write a single value to a register
             */
            void writeRegister(uint8_t reg, uint8_t value);

            /**
             * Read a single value from a register
             */
            uint8_t readRegister(uint8_t reg);

            /**
             * Write a multiple values to a register
             */
            void writeRegister(uint8_t reg, uint8_t* buff, uint8_t numValues);

            /**
             * Read a multiple values from a register
             */
            void readRegister(uint8_t reg, uint8_t* buff, uint8_t numValues);

            /**
             * Sends a command
             */
            void sendCommand(uint8_t command);

            void powerUp(bool transmit);
            void powerDown();

            void clearStatusReg();
    };
}

#endif