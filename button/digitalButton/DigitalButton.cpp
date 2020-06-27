#include "DigitalButton.hpp"

#include <Arduino.h>

namespace button
{
    DigitalButton::DigitalButton(uint8_t digitalPin)
    {
        digitalPin_ = digitalPin;
        currentState_ = ButtonState::INVALID;
        prevState_ = ButtonState::INVALID;
    }

    DigitalButton::~DigitalButton(){}

    void DigitalButton::update()
    {
        prevState_ = currentState_;

        uint8_t reading = digitalRead(digitalPin_);
        currentState_ = (reading == HIGH) ?
                        ButtonState::PRESSED :
                        ButtonState::NOT_PRESSED;
    }

    ButtonState DigitalButton::getState()
    {
        return currentState_;
    }

    ButtonTransition DigitalButton::getTransition()
    {
        if ((prevState_ != ButtonState::INVALID) &&
            (currentState_ != ButtonState::INVALID) &&
            (currentState_ != prevState_))
        {
            ButtonTransition transition = (currentState_ == ButtonState::PRESSED) ?
                                          ButtonTransition::PRESSED :
                                          ButtonTransition::RELEASED;

            return transition;
        }

        return ButtonTransition::NO_TRANSITION;
    }
}