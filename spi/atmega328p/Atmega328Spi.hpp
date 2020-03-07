#ifndef ATMEGA328_SPI_HPP
#define ATMEGA328_SPI_HPP

#include "spi/SpiDriver.hpp"

namespace spi
{
    enum SpiClock
    {
        CLOCK_DIV_2,
        CLOCK_DIV_4,
        CLOCK_DIV_8,
        CLOCK_DIV_16,
        CLOCK_DIV_32,
        CLOCK_DIV_64,
        CLOCK_DIV_128,
    };

    class Atmega328Spi : public SpiDriver
    {
        public:

            static void SendByte(uint8_t data);
            static uint8_t GetByte();

            // Construct as Master
            Atmega328Spi(uint8_t mosi,
                         uint8_t miso,
                         uint8_t clock,
                         uint8_t slaveSelect,
                         bool isMaster,
                         SpiClock clockSpeed = CLOCK_DIV_4,
                         SpiPolarity polarity = IDLE_LOW,
                         SpiPhase phase = SAMPLE_LEADING);

            ~Atmega328Spi();

            void enable() override;
            void disable() override;
            void reset() override;

            uint8_t transfer(uint8_t data, uint32_t delayMicroS);

            void write(uint8_t* buffer,
                       uint8_t numBytes,
                       uint32_t delayMicroS) override;

            void read(uint8_t* buffer,
                      uint8_t numBytes,
                      uint32_t delayMicroseconds) override;

            void writeAndReceive(uint8_t* writeBuffer,
                                 uint8_t* rcvBuffer,
                                 uint8_t numBytes,
                                 uint32_t delayMicroseconds) override;

            bool writeCollisionOccurred();

            void setSlaveInterruptCallback(uint8_t slaveSelect, void (*receiveHandler)(uint8_t));

            void selectSlave() override;
            void releaseSlave() override;

        private:
            uint8_t slaveSelect_;
            bool isMaster_;

            bool writeComplete();
            void setSpiClockSpeed(SpiClock clockSpeed);;
    };

    static uint8_t SlaveSelect = 0;
    static void (*SlaveReceiveHandler)(uint8_t) = nullptr;
}

#endif