#ifndef PHOTO_TRANSISTOR_HPP
#define PHOTO_TRANSISTOR_HPP

#include "filter/filter.hpp"

using namespace FilterSpace;

namespace photoTransistor{

    class PhotoTransistor{
        public:
            int pin_;
            Filter* pFilter_;

            PhotoTransistor(int pin, Filter* pFilter);

            void update();
            float getLightPercent();
    };
}


#endif