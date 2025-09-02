#include "RotaryEncoder.hpp"

#include "utilities/print/Print.hpp"

using namespace Dio;

const static float FULL_ROT_DEGREES = 360.0;

RotaryEncoder::RotaryEncoder(Dio::IDio* pChannelA,
                             Dio::IDio* pChannelB,
                             uint16_t countsPerRotation):
    pChannelA_(pChannelA),
    pChannelB_(pChannelB),
    prevALevel_(L_LOW),
    prevBLevel_(L_LOW),
    incAngle_(FULL_ROT_DEGREES / countsPerRotation),
    angle_(0),
    rotationCount_(0)
{
}

void RotaryEncoder::updateChannelA()
{
    Level currA = pChannelA_->read();
    Level currB = pChannelB_->read();

    if (currA == prevALevel_) return;
    
    if (currA == Level::L_LOW)
    {
        if (prevBLevel_ != currB)
        {
            if (currB == L_HIGH)
            {
                angle_ += incAngle_;
            }
            else
            {
                angle_ -= incAngle_;
            }
        }
    }

    prevALevel_ = currA;
    prevBLevel_ = currB;
}