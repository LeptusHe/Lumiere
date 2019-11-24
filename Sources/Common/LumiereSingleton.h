#pragma once
#include "Common/LumiereAssert.h"
#include "Common/LumiereMacro.h"

BEGIN_LUMIERE_NAMESPACE

template <typename T>
class Singleton {
public:
    static T& getInstance()
    {
        LUMIERE_ASSERT(mSingleton);
        return (*mSingleton);
    }

    static T* getInstancePtr()
    {
        LUMIERE_ASSERT(mSingleton);
        return mSingleton;
    }

public:
    Singleton()
    {
        LUMIERE_EXPECT(!mSingleton);
        mSingleton = reinterpret_cast<T*>(this);
        LUMIERE_ENSURE(mSingleton);
    }

    ~Singleton()
    {
        LUMIERE_EXPECT(mSingleton);
        mSingleton = nullptr;
        LUMIERE_ENSURE(!mSingleton);
    }

    Singleton(const Singleton<T>&) = delete;
    Singleton(const Singleton<T>&&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&&) = delete;

protected:
    static T *mSingleton;
};


template <typename T>
T *Singleton<T>::mSingleton = nullptr;;

END_LUMIERE_NAMESPACE