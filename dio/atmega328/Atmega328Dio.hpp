/**
 * Driver for Atmega328 processor Digital IO
 */
#ifndef ATMEGA328_DIO_HPP
#define ATMEGA328_DIO_HPP

#include "drivers/dio/IDio.hpp"

namespace Dio
{
    enum Mode: uint8_t
    {
        INPUT = 0,
        OUTPUT
    };

    enum class Port: uint8_t
    {
        B,
        C,
        D
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

        private:
            Port port_;
            uint8_t pin_;
            Mode mode_;
            bool inverted_;

            /**
             * Sets the mode of the pin, input or output
             * 
             * @param   mode    The mode to set the pin to
             */
            void setDirection(Mode mode);
    };
}

#endif