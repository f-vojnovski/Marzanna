#pragma once

#pragma once

#include "core/log.h"

// Assertions can be disabled by commenting out this line
#define MZ_ASSERTIONS_ENABLED 1

#ifdef MZ_ASSERTIONS_ENABLED
#if _MSC_VER
#include<intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif


#define MZ_ASSERT(expr)                                                             \
    {                                                                               \
        if(expr) {}                                                                 \
        else {                                                                      \
            Log::ReportAssertionFailure(#expr, "", __FILE__, __LINE__);             \
            debugBreak();                                                           \
        }                                                                           \
    }                                                                               \

#define MZ_ASSERT_MSG(expr, message)                                                \
    {                                                                               \
        if(expr) {}                                                                 \
        else {                                                                      \
            Log::ReportAssertionFailure(#expr, message, __FILE__, __LINE__);        \
            debugBreak();                                                           \
        }                                                                           \
    }                                                                               \

#ifdef _DEBUG
#define MZ_ASSERT_DEBUG(expr)                                                       \
        {                                                                           \
            if (expr) {}                                                            \
            else {                                                                  \
                Log::ReportAssertionFailure(#expr, "", __FILE__, __LINE__);         \
                debugBreak();                                                       \
            }                                                                       \
        }                                                                           
#else
#define MZ_ASSERT_DEBUG(expr)
#endif

#else
#define MZ_ASSERT(expr)
#define MZ_ASSERT_MSG(expr, message)
#define MZ_ASSERT_DEBUG(expr)

#endif