#pragma once

#if defined(__MINGW32__) || defined(__MINGW64__)
    #define LUMIERE_OS_MINGW
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    #define LUMIERE_OS_WINDOWS
#elif defined(__linux) || defined(__linux__)
    #define LUMIERE_OS_LINUX
#else
    #error unsupport platform
#endif