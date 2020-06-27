#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <Arduino.h>

static void assert(bool val, const char* text = nullptr)
{

    if (val) return;

    Serial.print("Assert failed");
    if (text != nullptr)
    {
        Serial.print(": ");
        Serial.print(text);
    }

    Serial.println(".");

    while(1)
    {
        // Infinite loop
    }
}

#endif