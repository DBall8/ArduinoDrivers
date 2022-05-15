#include "EepromManager.hpp"

#include <stdlib.h>

#include "drivers/assert/Assert.hpp"
#include "utilities/print/Print.hpp"

namespace Eeprom
{
    EepromManager::EepromManager(IEeprom* pEepromDriver,
                                 void* pData,
                                 uint16_t dataLength):
        pEepromDriver_(pEepromDriver),
        pData_((uint8_t*)pData),
        dataLength_(dataLength),
        currIndex_(0)
    {
        pShadowCopy_ = (uint8_t*)malloc(dataLength_);
        assertCustom(pShadowCopy_ != nullptr);
    }

    bool EepromManager::initialize()
    {
        bool success = true;
        for (uint16_t i=0; i<dataLength_; i++)
        {
            uint8_t value;
            if (pEepromDriver_->read(i, value))
            {
                // Set both copies to this value
                pData_[i] = value;
                pShadowCopy_[i] = value;
            }
            else
            {
                success = false;
            }
        }
        return success;
    }

    void EepromManager::update()
    {
        if (pData_[currIndex_] != pShadowCopy_[currIndex_])
        {
            // Value requires update
            if (!pEepromDriver_->write(currIndex_, pData_[currIndex_]))
            {
                // Eeprom was busy, stay on this index
                return;
            }

            // Update shadow copy
            pShadowCopy_[currIndex_] = pData_[currIndex_];
        }

        // Move to next value
        currIndex_ = (currIndex_ + 1 >= dataLength_) ? 0 : (currIndex_ + 1);
    }
}