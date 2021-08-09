#ifndef ATEMGA328_EEPROM_HPP
#define ATEMGA328_EEPROM_HPP

#include "drivers/eeprom/IEeprom.hpp"
#include "drivers/interrupt/IInterrupt.hpp"

namespace Eeprom
{
    class Atmega328Eeprom : public IEeprom
    {
        public:
            Atmega328Eeprom(Interrupt::IInterrupt* pIntControl);
            ~Atmega328Eeprom(){}

            bool write(uint16_t address, uint8_t value) override;
            bool read(uint16_t address, uint8_t& value) override;
            bool isBusy() override;

        private:
            Interrupt::IInterrupt* pIntControl_;
    };
}

#endif