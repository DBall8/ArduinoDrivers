#ifndef PHOTO_TRANSISTOR_HPP
#define PHOTO_TRANSISTOR_HPP

#include "drivers/adc/IAdc.hpp"
#include "utilities/filter/filter.hpp"

class PhotoTransistor{
    public:
        PhotoTransistor(Adc::IAdc* pAdc, Filter::IFilter* pFilter);
        ~PhotoTransistor(){}

        void update();
        float getLightPercent();

    private:
        Adc::IAdc* pAdc_;
        Filter::IFilter* pFilter_;
};


#endif