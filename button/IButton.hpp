#ifndef IBUTTON_HPP
#define IBUTTON_HPP

#include <stdint.h>

namespace Button
{
    enum class ButtonState: uint8_t 
    {
        PRESSED = 0,
        NOT_PRESSED,
        INVALID
    };

    enum class ButtonTransition: uint8_t 
    {
        PRESSED = 0,
        RELEASED,
        NO_TRANSITION
    };

    class IButton
    {
        public:
            virtual void update(){};
            virtual ButtonState getState() = 0;
            virtual ButtonTransition getTransition() = 0;
    };
}

#endif