#include "AnalogButton.hpp"

namespace Button
{
    AnalogButton::AnalogButton(ValidRange* validRanges, uint8_t numValidRanges):
        state_(ButtonState::INVALID),
        transition_(ButtonTransition::NO_TRANSITION),
        numValidRanges_(numValidRanges),
        validRanges_(validRanges)
    {
    }

    AnalogButton::~AnalogButton()
    {
    }

    void AnalogButton::update(uint16_t analogReading)
    {
        ButtonState newState = ButtonState::NOT_PRESSED;

        // Search through all valid ranges to see if this reading
        // falls within one
        for (uint8_t i=0; i<numValidRanges_; i++)
        {
            ValidRange range = validRanges_[i];
            if (range.analogMin <= analogReading &&
                range.analogMax >= analogReading)
            {
                // Valid range found, stop searching
                newState = ButtonState::PRESSED;
                break;
            }
        }

        // If button state was or is becoming invalid, mark as not transitioning
        if (newState == ButtonState::INVALID ||
            state_ == ButtonState::INVALID)
        {
            state_ = newState;
            transition_ = ButtonTransition::NO_TRANSITION;
            return;
        }

        // If button state has changed, mark as transitioned
        if (newState != state_)
        {
            transition_ = (newState == ButtonState::PRESSED) ?
                        ButtonTransition::PRESSED :
                        ButtonTransition::RELEASED;
        }
        else
        {
            // Button state did not change, no transition is flagged
            transition_ = ButtonTransition::NO_TRANSITION;
        }

        state_ = newState;
    }

    ButtonState AnalogButton::getState()
    {
        return state_;
    }

    ButtonTransition AnalogButton::getTransition()
    {
        return transition_;
    }

}