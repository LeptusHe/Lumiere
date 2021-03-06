#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <fmt/format.h>
#include "Common/LumiereMacro.h"

BEGIN_LUMIERE_NAMESPACE

inline std::string ToUpper(const std::string& text)
{
    std::string result;
    std::transform(std::begin(text), std::end(text), std::back_inserter(result), ::toupper);
    return result;
}


inline std::string ToLower(const std::string& text)
{
    std::string result;
    std::transform(std::begin(text), std::end(text), std::back_inserter(result), ::tolower);
    return result;
}

std::vector<float> ParseFloatArray(const std::string& text);
std::vector<std::string> SplitStringBySpace(const std::string& text);

#define LUMIERE_STRING_FMT(fmtStr, ...) fmt::format(fmtStr, __VA_ARGS__)

END_LUMIERE_NAMESPACE
