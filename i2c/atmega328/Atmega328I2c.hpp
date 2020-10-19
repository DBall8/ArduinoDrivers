#ifndef ATMEGA328_I2C_HPP
#define ATMEGA328_I2C_HPP

#include "drivers/i2c/II2c.hpp"
#include "drivers/interrupt/IInterrupt.hpp"
#include "drivers/timer/SoftwareTimer.hpp"

/**
 * NOTE:
 * Slave clock frequency must be 16 times higher SCL
 * SCL Freq = CPU_F / (16 + 2(TWBR) * (Prescaler))
 */

namespace I2c{

    enum class I2cBitRate: uint8_t
    {
        BR_100_KBPS,
        BR_400_KBPS
    };

    class Atmega328I2c : public II2c
    {
        public:
            /**
             * Constructor for i2c as master
             * @param   bitRate         Transmission speed to use
             * @param   pTimeoutTimer   If any step of an i2c communication takes longer than the timer's period
             *                          then the communication will be dropped and the i2c reset
             * @param   usePullup       If true, use the Atmega's internal pullup resistors for the i2c lines
             */
            Atmega328I2c(I2cBitRate bitRate,
                        Timer::SoftwareTimer* pTimeoutTimer = nullptr,
                        bool usePullup = false);

            /**
             * Destructor
             */
            ~Atmega328I2c();

            /**
             * Enables the i2c
             */
            void enable() override;
            /**
             * Disables the i2c
             */
            void disable() override;

            /**
             * Transmit data to a slave
             * @param   slaveId     ID of the slave to send to
             * @param   dataArray   Array of data to send
             * @param   numBytes    Number of bytes in the dataArray to send
             * @return  True if successful
             */
            bool transmit(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes) override;
            /**
             * Receive data from a slave
             * @param   slaveId     ID of the slave to receive from
             * @param   dataArray   Array to place received data in
             * @param   numBytes    Number of bytes to receive from the slave
             * @return  True if successful
             */
            bool receive(uint8_t slaveId, uint8_t* dataArray, uint16_t numBytes) override;

            /**
             * Transmit a single byte to a slave
             * @param   slaveID     ID of the slave to transmit to
             * @param   data        Single byte to transmit
             * @return  Return true if successful
             */
            bool transmit(uint8_t slaveId, uint8_t data) override;

            /**
             * Receive a single byte from a slave
             * @param   slaveID     ID of the slave to receive from
             * @param   data        Reference to read the byte to
             * @return  Return true if successful
             */
            bool receive(uint8_t slaveId, uint8_t& data) override;

        private:
            // Timeout timer for the wait() call
            Timer::SoftwareTimer* pTimeoutTimer_;

            // Tracks if i2c has been enabled
            bool enabled_;

            /**
             * Set the bit rate for the i2c (master controls this)
             * @param   bitRate Supported transmission speed
             */
            void setBitRate(I2cBitRate bitRate);

            /**
             * Send a start signal, this is a blocking call
             * @return  True if the start signal was successfully sent
             */
            bool start();
            /**
             * Send a stop signal
             */
            void stop();
            /**
             * Resets i2c to initial conditions
             */
            void reset();
            /**
             * Send the slave address across i2c
             * @param   slaveId     ID to send
             * @param   read        If true, set the read bit. If false, leave it as 0 indicating a write
             * @return  True if a slave ACKed the slave ID
             */
            bool sendSlaveAddress(uint8_t slaveId, bool read);
            /**
             * Send a byte of data
             * @param   data    The byte to send
             * @return  True if the byte was ACKed by the slave
             */
            bool sendData(uint8_t data);
            /**
             * Receive a byte of data from a slave
             * @param   data    Reference to location to place received data
             * @param   isLast  If true, this is the last byte to receive, and we will send a NACK to
             *                  indicate this to the slave. Otherwise, we will ACK received data
             * @return  True if data was received, and we responded as desired (ACKed, or NACKed if last byte)
             */
            bool receiveData(uint8_t& data, bool isLast);

            /**
             * Retrieve the current status
             * @return Value of the i2c status register, masked to just the status bits
             */
            uint8_t getStatus();

            /**
             * Sits in a while loops until the last i2c transaction is completed
             * If a timeout timer was set, will return false when the timer expires
             * @return  True when the previous i2c transaction finished, or false
             *          if the timeout timer expires first
             */
            bool wait();
    };
}
#endif