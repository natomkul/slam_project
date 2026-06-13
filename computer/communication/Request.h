#pragma once

#include <cstdint>

enum class RequestType : uint8_t
{
    move = 0,
    rotate = 1
};

#pragma pack(push, 1)
struct RequestWire
{
    RequestType type;
    float distanceInM;
    float rotationAngle;
};
#pragma pack(pop)

static_assert(sizeof(RequestWire) == 9, "RequestWire must match robot firmware layout");
