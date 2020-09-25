/**
 * Interface for LCD display modules
 */
#ifndef ILCD_HPP
#define ILCD_HPP

#include <stdint.h>

namespace Lcd
{
    class ILcd
    {
        public:
            virtual bool initialize(){ return true; }
            virtual bool setCursor(uint8_t row, uint8_t column) = 0;
            virtual bool display(const char* string, uint8_t strLength) = 0;
            virtual bool display(uint8_t characterCode) = 0;
            virtual bool clear() = 0;
    };
}


#endif