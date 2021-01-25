/**
 * A class for running serial communication over non-uart enabled pins
 * on the Atmega328 processor
 */
#ifndef ATMEGA328_SOFTWARE_SERIAL_HPP
#define ATMEGA328_SOFTWARE_SERIAL_HPP

#include "drivers/serial/ISerial.hpp"
#include "drivers/dio/IDio.hpp"
#include "drivers/interrupt/IInterrupt.hpp"

#include "utilities/circular_queue/CircularQueue.hpp"

namespace Serial
{
    class Atmega328SoftwareSerial : public ISerial
    {
        /**
         * Constructor
         * @param   pRxPin      Pointer to the pin that will receive data into the processor
         * @param   pTxPin      Pointer to the pin that will transmit data out of the processor
         * @param   pIntControl Object to enable/disable interrupts
         * @param   baudRate    Bits transfered per second
         * @param   fCpu        Frequency of the processor's clock
         * @param   rxBuffer    Buffer where incoming data will be stored
         * @param   rxBufferLen Size of the rxBuffer
         */
        public:
            Atmega328SoftwareSerial(Dio::IDio* pRxPin,
                                    Dio::IDio* pTxPin,
                                    Interrupt::IInterrupt* pIntControl,
                                    uint32_t baudRate,
                                    uint32_t fCpu,
                                    uint8_t* rxBuffer,
                                    uint16_t rxBufferLen);

            ~Atmega328SoftwareSerial(){}

            /**
             * Set up the processor to be ready to receive and transmit
             */
            void initialize();

            /**
             * Check if the serial driver has new received data
             * @return  True if there is newly received data
             */
            bool isDataAvailable() override;

            /**
             * Write out data
             * @param   buff        Buffer of data to transmit
             * @param   numBytes    Number of bytes to transmit
             */
            void write(uint8_t* buff, uint16_t numBytes) override;

            /**
             * Read data that has been received
             * @param   buff        Buffer for received data to be copied to
             * @param   numBytes    Maximum number of received data to copy to buff
             * @return  Number of bytes copied to buff
             */    
            uint16_t read(uint8_t* buff, uint16_t numBytes) override;

            /**
             * Empty the rx buffer
             */
            void flush();

        private:
            CircularQueue<uint8_t> rxBuffer_;   // Circular queue for receiving data

            Dio::IDio* pRxPin_; // RX pin
            Dio::IDio* pTxPin_; // TX pin
            Interrupt::IInterrupt* pIntControl_;    // Interrupt enable/disable object

            int16_t txDelayQuadCycles_;     // Number of 4-cpu-cycles to delay between writing each bit on TX
            int16_t rxCenteringQuadCycles_; // Number of 4-cpu-cyles to delay before reading the first bit, to center in the middle of the bit pulse
            int16_t rxInterBitQuadCycles_;  // Number of 4-cpu-cycles to delay between reading each bit on RX

            bool dataAvailable_;    // When true, there is data that has been received but not claimed by read()
            bool rxOverflow_;       // When true, data was received when the RX queue was already full

            /**
             * Writes out a single byte on the TX pin
             * @param   byte    Byte to transmit
             */
            void writeByte(uint8_t byte);

            /**
             * Receive a single byte on the RX pin, writes it to the RX queue
             */
            void receiveByte();

            // Interrupt handler for data change events on the RX pin
            static void handleRxInterrupt();
    };
}

#endif