#pragma once
#include "Common/LumiereMacro.h"
#include "Common/LumierePlatform.h"


#if defined(LUMIERE_OS_WINDOWS) && !NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


inline void installMemoryLeakDetector()
{
#if defined(DEBUG) | defined(_DEBUG) | !defined(NDEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}


#if defined(_DEBUG) | !defined(NDEBUG)
    #define LUMIERE_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define LUMIERE_DELETE delete
    #define LUMIERE_NEW_ARRAY(T, size) new T[size];
    #define LUMIERE_DELETE_ARRAY(ptr)  delete[] ptr
#else
    #define LUMIERE_NEW new
    #define LUMIERE_DELETE delete
    #define LUMIERE_NEW_ARRAY(T, size) new T[size]
    #define LUMIERE_DELETE_ARRAY(ptr)  delete[] ptr;
#endif

#else

#define LUMIERE_NEW new
#define LUMIERE_DELETE delete
#define LUMIERE_NEW_ARRAY(T, size) new T[size]
#define LUMIERE_DELETE_ARRAY(ptr)  delete[] ptr;

#endif