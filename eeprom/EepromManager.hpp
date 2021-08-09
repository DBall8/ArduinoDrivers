#ifndef EEPROM_MANAGER_HPP
#define EEPROM_MANAGER_HPP

#include "IEeprom.hpp"

namespace Eeprom
{
    class EepromManager
    {
        public:
            EepromManager(IEeprom* pEepromDriver,
                          void* pData,
                          uint16_t dataLength);

            bool initialize();
            void update();

        private:
            IEeprom* pEepromDriver_;
            uint8_t* pData_;
            uint8_t* pShadowCopy_;
            uint16_t dataLength_;

            uint16_t currIndex_;
    };
}

#endif