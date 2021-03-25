#ifndef ATMEGA328_ASYNCH_UART_HPP
#define ATMEGA328_ASYNCH_UART_HPP

#include "drivers/serial/atmega328/Atmega328Uart.hpp"
#include "utilities/circular_queue/CircularQueue.hpp"
#include "drivers/interrupt/IInterrupt.hpp"

namespace Serial
{
    class Atmega328AsynchUart : public Atmega328Uart
    {
        public:
            /**
             * Constructor
             * @param   txBuffer            Pointer to buffer to store outgoing data in
             * @param   rxBuffer            Pointer to buffer to store incoming data in
             * @param   txLength            Length of txBuffer
             * @param   rxLength            Length of rxBuffer
             * @param   baudRate            UART transmission rate to use
             * @param   pInterruptControl   Pointer to global interrupt controller
             * @param   enableParity        Enable parity
             * @param   polarity            Set polarity
             */
            Atmega328AsynchUart(uint8_t* txBuffer, 
                                uint8_t* rxBuffer, 
                                uint16_t txLength,
                                uint16_t rxLength, 
                                BaudRate baudRate,
                                uint32_t fCpu,
                                Interrupt::IInterrupt* pInterruptControl,
                                bool enableParity = false, 
                                bool polarity = false);
            ~Atmega328AsynchUart();

            /**
             * Set up uart, must be done after static initializtion
             */
            void initialize() override;

            /**
             * Returns true if there is incoming data to read
             */
            bool isDataAvailable() override;

            /**
             * Non blocking write, puts data in a buffer to be written out
             * 
             * @param   buff        Buffer containing new data to write
             * @param   numBytes    Number of bytes in buff
             */
            virtual void write(const uint8_t* buff, uint16_t numBytes) override;

            /**
             * Non-blocking read, retrieves any data read and buffered from the uart
             * 
             * @param   buff        Buffer to put read bytes in
             * @param   numBytes    Size of buff, maximum number of bytes to read
             * @return  Returns the number of bytes read, up to numBytes
             */
            virtual uint16_t read(uint8_t* buff, uint16_t numBytes) override;

            /**
             * Empty all contents of the write buffer to the uart
             */
            void flush() override;

            /**
             * Callback for the Data Register Empty interrupt
             */
            static void HanleDataRegisterEmpty();

            /**
             * Callback for the Data read complete interrupt
             */
            static void HanleRxDataAvailable();

        private:

            // Static copies for use in interrupt handling
            static CircularQueue<uint8_t>* pTxBuffer_;
            static CircularQueue<uint8_t>* pRxBuffer_;
            static Interrupt::IInterrupt* pInterruptControl_;

            CircularQueue<uint8_t> txBuffer_;   // Stores outgoing bytes
            CircularQueue<uint8_t> rxBuffer_;   // Stores incoming bytes
            uint16_t txLength_; // Length of txBuffer_
            uint16_t rxLength_; // Length of rxBuffer_
            bool txBufferFull_; // True when the TX buffer has overfilled and must be emptied
    };
}

#endif