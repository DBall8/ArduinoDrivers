#ifndef IEEPROM_HPP
#define IEEPROM_HPP

#include <stdint.h>

namespace Eeprom
{
    class IEeprom
    {
        public:
            virtual bool write(uint16_t address, uint8_t value) = 0;
            virtual bool read(uint16_t address, uint8_t& value) = 0;
            virtual bool isBusy();
    };
}

#endif