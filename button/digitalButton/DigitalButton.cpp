#include "DigitalButton.hpp"

using namespace Dio;
using namespace Timer;

namespace Button
{
    DigitalButton::DigitalButton(IDio* pDio, SoftwareTimer* pDebounceTimer)
    {
        pDio_ = pDio;
        pDebounceTimer_ = pDebounceTimer;
        currentState_ = ButtonState::INVALID;
        prevState_ = ButtonState::INVALID;
    }

    DigitalButton::~DigitalButton(){}

    void DigitalButton::update()
    {
        prevState_ = currentState_;

        Level reading = pDio_->read();
        ButtonState actualState = (reading == Level::L_HIGH) ?
                        ButtonState::PRESSED :
                        ButtonState::NOT_PRESSED;

        if (pDebounceTimer_ != nullptr)
        {
            if (pDebounceTimer_->isEnabled() && !pDebounceTimer_->hasOneShotPassed())
            {
                // Do not upate current stateuntil the debounce timer has finished
                return;
            }
            else if ((actualState == ButtonState::PRESSED) &&
                (prevState_ != ButtonState::PRESSED))
            {
                pDebounceTimer_->enable();
            }
        }

        currentState_ = actualState;
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