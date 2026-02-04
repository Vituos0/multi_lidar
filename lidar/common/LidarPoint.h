#pragma once

#include <cstdint> 

struct LidarPoint
{
    float angle;     // degree
    float distance;  // meter
    uint8_t intensity;
};
