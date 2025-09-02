#ifndef ROTARY_ENCODER_HPP
#define ROTARY_ENCODER_HPP

#include "drivers/dio/IDio.hpp"

struct EncoderState
{
    Dio::Level channelALevel;
    Dio::Level channelBLevel;
};

class RotaryEncoder
{
    public:
        RotaryEncoder(Dio::IDio* pChannelA,
                      Dio::IDio* pChannelB,
                      uint16_t countsPerRotation);

        ~RotaryEncoder(){}

        float getAngle() { return angle_; }
        uint32_t getRotationCount() { return rotationCount_; }
        void reset(){ rotationCount_ = 0; angle_ = 0; }

        void updateChannelA();

    private:
        Dio::IDio* pChannelA_;
        Dio::IDio* pChannelB_;
        Dio::Level prevALevel_;
        Dio::Level prevBLevel_;
        float incAngle_;

        float angle_;
        uint32_t rotationCount_;
};
#endif