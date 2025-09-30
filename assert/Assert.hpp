#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "utilities/print/Print.hpp"

void assertCustom(bool val, const char* text = nullptr);
void assert(bool val, const char* text = nullptr);

#endif