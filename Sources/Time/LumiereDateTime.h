#pragma once
#include <string>
#include "Common/LumierePlatform.h"
#include "Common/LumiereMacro.h"

BEGIN_LUMIERE_NAMESPACE

class DateTime {
public:
    static DateTime now();

public:
    ~DateTime() = default;
    int getYear() const  { return mYear; }
    int getMonth() const { return mMonth;}
    int getDay() const   { return mDay; }
    int getHour() const  { return mHour; }
    int getMinute() const { return mMinute; }
    int getSecond() const { return mSecond; }
    int getMillisecond() const { return mMillisecond; }

private:
    DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

private:
    int mYear;
    int mMonth;
    int mDay;
    int mHour;
    int mMinute;
    int mSecond;
    int mMillisecond;
};

END_LUMIERE_NAMESPACE
