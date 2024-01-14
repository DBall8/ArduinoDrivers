#include "Atmega328SoftwareSerial.hpp"
#include "util/delay_basic.h"

#include "utilities/print/Print.hpp"
#include "drivers/assert/Assert.hpp"

using namespace Dio;
using namespace Interrupt;
using namespace Timer;

namespace SerialComm
{
    const static uint8_t WRITE_BIT_CPU_CYCLES = 86; // CPU cycles taken to write a bit value to a pin

    const static uint8_t RX_INT_OPEN_CPU_CYCLES = 240;  // Number of CPU cycles between the RX pin going low, and the first bit is read on the rx pin
    const static uint8_t READ_BIT_CPU_CYCLES = 106;     // Number of CPU cycles taken to perform a pin read

    /**
     * Delays for units of 4 cpu cycles
     * @param   quadCpuCycles   Number of cpu cycles to delay, divided by 4
     */
    inline void accurateDelay(uint16_t quadCpuCycles)
    {
        _delay_loop_2(quadCpuCycles);
    }

    const static uint8_t MAX_NUM_SOFT_SERIAL = 4;   // Max number of software serials that are supported at one time
    static uint8_t numActiveSerialConns = 0;        // Number of running software serials
    static Atmega328SoftwareSerial* activeSerialConns[MAX_NUM_SOFT_SERIAL]; // List of active software serial objects

    Atmega328SoftwareSerial::Atmega328SoftwareSerial(Dio::IDio* pRxPin,
                                                     Dio::IDio* pTxPin,
                                                     Interrupt::IInterrupt* pIntControl,
                                                     uint32_t baudRate,
                                                     uint32_t fCpu,
                                                     uint8_t* rxBuffer,
                                                     uint16_t rxBufferLen,
                                                     SoftwareTimer* pTimeoutTimer) :
        rxBuffer_(rxBuffer, rxBufferLen),
        pRxPin_(pRxPin),
        pTxPin_(pTxPin),
        pIntControl_(pIntControl),
        dataAvailable_(false),
        rxOverflow_(false)
    {
        // Desired amount of time for each bit value to be held on each pin
        uint32_t cpuCyclesPerBit = (fCpu / baudRate);

        // All delays are in units of 4 clock cycles, since our delay function works on a 4 cpu cycle delay

        txDelayQuadCycles_ = cpuCyclesPerBit / 4;       // Number of clock cycles to hold each bit
        txDelayQuadCycles_ -= WRITE_BIT_CPU_CYCLES / 4; // Subtract the cycles of overhead needed to set the pin

        // Time to wait between each bit for reading, calculated as the baud rate minues the overhead to perform the pin read
        rxInterBitQuadCycles_ = (cpuCyclesPerBit - READ_BIT_CPU_CYCLES) / 4;

        // Time to wait before the first read, in order to read as close to the center of the pulse as possible
        // calculated as half of the baud rate minus the overhead for all logic for entering the RX interrupt
        rxCenteringQuadCycles_ = ((3 * cpuCyclesPerBit / 2) - RX_INT_OPEN_CPU_CYCLES) / 4;

        // Don't go below 0, if we hit 0 its likely it will not work
        if (rxCenteringQuadCycles_ < 0) rxCenteringQuadCycles_ = 0;
        if (rxInterBitQuadCycles_ < 0) rxInterBitQuadCycles_ = 0;
        if (txDelayQuadCycles_ < 0) txDelayQuadCycles_ = 0;

        setTimeoutTimer(pTimeoutTimer);
    }

    void Atmega328SoftwareSerial::initialize()
    {
        // Set up interrupt for receiving

        // Add this object to the static array for handling RX interrupts
        assertCustom(numActiveSerialConns < MAX_NUM_SOFT_SERIAL);
        activeSerialConns[numActiveSerialConns] = this;
        numActiveSerialConns++;

        // Enable pin change interrupt on RX
        pRxPin_->enableInterrupt(handleRxInterrupt);

        // Make sure we are in tx standby by pullng high
        if (pTxPin_->read() == L_LOW)
        {
            // Hold low first to send stop signal
            accurateDelay(txDelayQuadCycles_);
            pTxPin_->set(L_HIGH);
        }
    }

    bool Atmega328SoftwareSerial::isDataAvailable()
    {
        return dataAvailable_;
    }

    void Atmega328SoftwareSerial::write(const uint8_t* buff, uint16_t numBytes)
    {
        // Write each byte out, this is a blocking write
        for (uint16_t i=0; i<numBytes; i++)
        {
            writeByte(buff[i]);
        }
    }

    uint16_t Atmega328SoftwareSerial::read(uint8_t* buff, uint16_t numBytes)
    {
        // If there is now new data received since the last read(), return 0
        if (!dataAvailable_)
        {
            return 0;
        }

        // Copy each byte in the RX queue into buff, until either numBytes have been copied or
        // the RX queue is empty
        uint16_t bytesRead = 0;
        for (uint16_t i=0; i<numBytes; i++)
        {
            if (rxBuffer_.isEmpty())
            {
                // Ran out of data to read
                dataAvailable_ = false;
                return bytesRead;
            }

            buff[i] = rxBuffer_.pop();
            bytesRead++;
        }

        // Update data available flag
        dataAvailable_ = !rxBuffer_.isEmpty();

        // Notify if an rx overflow event occurred
        if (rxOverflow_)
        {
            PRINTLN("SS Overflow");
            rxOverflow_ = false;
        }

        return bytesRead;
    }

    void Atmega328SoftwareSerial::flush()
    {
        rxBuffer_.flush();
        dataAvailable_ = false;
    }

    void Atmega328SoftwareSerial::writeByte(uint8_t byte)
    {
        // Start timing critical zone
        pIntControl_->pauseInterrupts();

        // Start TX by pulling low
        pTxPin_->set(L_LOW);
        accurateDelay(txDelayQuadCycles_);

        // Transmit each bit, held for the required time
        // LSB to MSB
        for (uint8_t i=0; i<8; i++)
        {
            pTxPin_->set(byte & 1);
            byte >>= 1;

            accurateDelay(txDelayQuadCycles_);
        }

        // End by pulling high
        pTxPin_->set(L_HIGH);
        accurateDelay(txDelayQuadCycles_);

        // Re-enable interrupts
        pIntControl_->resumeInterrupts();
    }

    void Atmega328SoftwareSerial::receiveByte()
    {
        uint8_t data = 0;

        // Start timing critical zone
        pIntControl_->pauseInterrupts();

        // Center ourselves within the pulse
        accurateDelay(rxCenteringQuadCycles_);

        // Read each bit after waiting the required time
        // LSB to MSB
        for (uint8_t i=0; i<8; i++)
        {
            data >>= 1;
            data |= ((uint8_t)pRxPin_->read()) << 7; // Shift 7 to place at MSB, which will then be shifted as we read

            accurateDelay(rxInterBitQuadCycles_);
        }

        // Do not save byte if our buffer is full, but instead set overflow flag
        if (rxBuffer_.isFull())
        {
            rxOverflow_ = true;
        }
        else
        {
            // Store new byte
            rxBuffer_.push(data);
            dataAvailable_ = true;
        }
        
        // Ensure we have stopped
        // while (pRxPin_->read() == L_LOW)
        // {}

        // Re-enable interrupts, timing critical region has ended
        pIntControl_->resumeInterrupts();
    }

    void Atmega328SoftwareSerial::handleRxInterrupt()
    {
        for (uint8_t i=0; i<numActiveSerialConns; i++)
        {
            // Find the serial whose RX pin is low, indicating a start bit
            Atmega328SoftwareSerial* pSerial = activeSerialConns[i];
            if (pSerial->pRxPin_->read() == L_LOW)
            {
                pSerial->receiveByte();
                return;
            }
        }

    }
}