#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "utilities/print/Print.hpp"

static void assertCustom(bool val, const char* text = nullptr)
{

    if (val) return;

    PRINT("Assert failed");

#ifdef PRINT_ASSERT
    if (text != nullptr)
    {
        PRINT(": ");
        PRINT(text);
    }

    PRINTLN(".");
#endif

    // Flush uart before entering busy loop
    PRINT_FLUSH();

    while(1)
    {
        // Infinite loop
    }
}
#endif