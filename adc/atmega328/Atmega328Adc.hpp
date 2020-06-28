#ifndef ATMEGA328_ADC_HPP
#define ATMEGA328_ADC_HPP

#include "drivers/adc/IAdc.hpp"

namespace adc
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
            Atmega328Adc(Atmega328Channel channel, Prescaler prescaler, Reference reference);
            ~Atmega328Adc(){}

            void enable() override;
            void disable() override;
            bool read(uint16_t& value) override;

        private:
            Atmega328Channel channel_;
            Reference reference_;

            void selectChannel();
    };
}

#endif