/**
 * Driver for Atmega328 processor Digital IO
 */
#ifndef ATMEGA328_DIO_HPP
#define ATMEGA328_DIO_HPP

#include "drivers/dio/IDio.hpp"

namespace Dio
{
    enum class Port: uint8_t
    {
        B = 0,
        C = 1,
        D = 2
    };

    // Number of pins per port
    const static uint8_t NUM_PINS = 8;

    class Atmega328Dio: public IDio
    {
        public:
            /**
             * Constructor
             * 
             * @param   port            The port the digital io pin is located on
             * @param   pin             The pin number of the io pin
             * @param   mode            Input or output mode
             * @param   initialLevel    Initial level for output pins
             * @param   inverted        If true, output as high when set to low
             * @param   usePullup       If set as input, uses an internal pullup resistor when true
             */
            Atmega328Dio(Port port,
                         uint8_t pin,
                         Mode mode,
                         Level initialLevel,
                         bool inverted,
                         bool usePullup);
            ~Atmega328Dio(){}

            /**
             * Sets the output level of the digital IO
             * 
             * @param   level   The level to set the pin to
             * @return  True if successful
             */
            bool set(Level level) override;

            /**
             * Sets the output level of the digital IO
             * Lightweight version for timing critical operations, sets raw value
             * 
             * @param   level   Sets low if 0, high if not 0
             */
            void set(uint8_t level) override;

            /**
             * Toggles the output of the pin, if it was HIGH it becomes LOW
             * and vice versa
             * 
             * @return  True if successful
             */
            bool toggle() override;

            /**
             * Reads the level of the digital IO
             * 
             * @return  The current digital IO reading
             */
            Level read() override;

            /**
             * Change the pin into an output pin
             * @param level Level to start outputing
             */
            void setOutputMode(Level level) override;

            /**
             * Change the pin into an input pin
             * @param usePullup If true, enable an internal pullup resistor
             */
            void setInputMode(bool usePullup) override;

            /**
             * Enable pin change interrupts for this pin
             * IMPORTANT: Currently only one interrupt handler can be set per PORT
             * Any logic to differentiate between different pins must be inside the interrupt handler
             * @param pIntHandler Pointer to the function to run on interrupt, or nullptr to not set a function
             */
            void enableInterrupt(void (*pIntHandler)(void) = nullptr) override;

            /**
             * Disable pin change interrupts for this pin
             */
            void disableInterrupt() override;

        private:
            volatile Port port_;
            volatile uint8_t pin_;
            Mode mode_;
            bool inverted_;

            /**
             * Sets the mode of the pin, input or output
             * 
             * @param   mode    The mode to set the pin to
             */
            void setDirection(Mode mode);
            void enterModeTransition();

            static uint8_t getPcintNumber(Port port, uint8_t pin);
    };
}

#endif