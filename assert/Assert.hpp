#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "utilities/print/Print.hpp"

static void assert(bool val, const char* text = nullptr)
{

    if (val) return;

    PRINT("Assert failed");
    if (text != nullptr)
    {
        PRINT(": ");
        PRINT(text);
    }

    PRINTLN(".");

    // Flush uart before entering busy loop
    PRINT_FLUSH();

    while(1)
    {
        // Infinite loop
    }
}

#endif