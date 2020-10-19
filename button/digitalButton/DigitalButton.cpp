#include "DigitalButton.hpp"

using namespace Dio;

namespace Button
{
    DigitalButton::DigitalButton(IDio* pDio)
    {
        pDio_ = pDio;
        currentState_ = ButtonState::INVALID;
        prevState_ = ButtonState::INVALID;
    }

    DigitalButton::~DigitalButton(){}

    void DigitalButton::update()
    {
        prevState_ = currentState_;

        Level reading = pDio_->read();
        currentState_ = (reading == Level::L_HIGH) ?
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