#include "Atmega328Adc.hpp"
#include <avr/io.h>

namespace adc
{
    Atmega328Adc::Atmega328Adc(Atmega328Channel channel, Prescaler prescaler, Reference reference):
        channel_(channel),
        reference_(reference)
    {
        // Disable digital IO for this channel
        DIDR0 |= (0x01 << channel);

        // Set the prescaler bits
        ADCSRA |= prescaler;
    }

    void Atmega328Adc::enable()
    {
        // NOTE: enables all channels
        ADCSRA |= (0x01 << ADEN);
    }

    void Atmega328Adc::disable()
    {
        // NOTES: disables all channels
        ADCSRA &= ~(0x01 << ADEN);
    }

    bool Atmega328Adc::read(uint16_t& value)
    {
        selectChannel();

        // Start conversion
        ADCSRA |= (0x01 << ADSC);

        while (!(ADCSRA & (0x01 << ADIF)))
        {
            // Wait for ADC conversion to complete
        }
 
        // MUST read ADCL first
        uint8_t lowByte = ADCL;
        uint8_t highByte = ADCH;

        value = (highByte * 0x100) + lowByte;

        // Write to the complete flag to reset it
        ADCSRA |= (0x01 << ADIF);

        return true;
    }

    void Atmega328Adc::selectChannel()
    {
        // Keep reference bits in ADMUX intact
        ADMUX = (reference_ << REFS0) | channel_;
    }
}