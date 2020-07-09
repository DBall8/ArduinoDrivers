#ifndef PHOTO_TRANSISTOR_HPP
#define PHOTO_TRANSISTOR_HPP

#include "drivers/adc/IAdc.hpp"
#include "utilities/filter/filter.hpp"

namespace photoTransistor{

    class PhotoTransistor{
        public:
            PhotoTransistor(adc::IAdc* pAdc, filter::Filter* pFilter);
            ~PhotoTransistor(){}

            void update();
            float getLightPercent();

        private:
            adc::IAdc* pAdc_;
            filter::Filter* pFilter_;
    };
}


#endif