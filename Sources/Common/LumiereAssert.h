#pragma once
#include <cassert>
#include "LumiereConfig.h"

#define LUMIERE_ASSERT(expression) assert(expression)
#define LUMIERE_EXPECT(expression) assert(expression)
#define LUMIERE_ENSURE(expression) assert(expression)

#ifdef _MSC_VER
#define SHOULD_NOT_GET_HERE() __assume(0)
#else
#define SHOULD_NOT_GET_HERE() __builtin_unreachable()
#endif