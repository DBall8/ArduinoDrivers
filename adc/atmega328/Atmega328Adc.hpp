/**
 * Driver for the Atmega328 analog to digital converter
 */
#ifndef ATMEGA328_ADC_HPP
#define ATMEGA328_ADC_HPP

#include "drivers/adc/IAdc.hpp"

namespace Adc
{
    enum Atmega328Channel: uint8_t
    {
        ADC_0 = 0,
        ADC_1,
        ADC_2,
        ADC_3,
        ADC_4,
        ADC_5,
        ADC_6,
        ADC_7
    };

    enum Prescaler: uint8_t
    {
        DIV_2 = 0,
        DIV_2_b,
        DIV_4,
        DIV_8,
        DIV_16,
        DIV_32,
        DIV_64,
        DIV_128
    };

    enum Reference: uint8_t
    {
        AREF = 0,
        AVCC,
        RESERVED,
        INTERNAL_1V
    };

    class Atmega328Adc: public IAdc
    {
        public:
            /**
             * Constructor
             * 
             * @param   channel     The adc channel to use, A0 through A7
             * @param   prescaler   Prescaler to divide the input clock by, requires between 200 and 50 kHz
             * @param   reference   The reference to use for measurements, AREF, AVcc, or internal 1.1v
             */
            Atmega328Adc(Atmega328Channel channel, Prescaler prescaler, Reference reference);
            ~Atmega328Adc(){}

            /**
             * Enables the ADC (all channels)
             */
            void enable() override;

            /**
             * Disables the ADC (all channels)
             */
            void disable() override;

            /**
             * Reads from the ADC channel with 10 bit resolution
             * 
             * @param   bool   True if read was successful
             * @return  Value of the channel reading
             */
            uint16_t read(bool* success = nullptr) override;

        private:
            Atmega328Channel channel_;
            Reference reference_;

            /**
             * Selects the channel to read from
             */
            void selectChannel();
    };
}

#endif