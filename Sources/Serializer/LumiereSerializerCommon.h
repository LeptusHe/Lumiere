#pragma once
#include <cstdint>
#include "Common/LumiereMacro.h"

BEGIN_LUMIERE_NAMESPACE

constexpr uint32_t SERIALIZER_HEADER_CHECKER = 0x1309262;
constexpr char SerializerVersionInfo[] = "[LumiereSerializer_v1.00]";

enum class Endian {
    LITTLE = 0,
    BIG = 1,
    COUNT = 2,
    DEFAULT = LITTLE
};

END_LUMIERE_NAMESPACE
