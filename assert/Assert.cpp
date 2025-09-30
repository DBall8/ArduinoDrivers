#include "Assert.hpp"

void assertCustom(bool val, const char* text)
{
#ifndef PRINT_ASSERT
    (void)text;
#endif

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

void assert(bool val, const char* text)
{
    assertCustom(val, text);
}