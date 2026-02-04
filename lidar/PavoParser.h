#pragma once
#include <vector>
#include <cstdint>
#include "lidar/common/LidarPoint.h"

class PavoParser
{
public:
    static constexpr int PACKET_SIZE = 126;
    static constexpr int GROUP_COUNT = 12;
    static constexpr int GROUP_SIZE  = 10;

    static bool parse(const uint8_t* data,
                      size_t len,
                      std::vector<LidarPoint>& outPoints);

private:
#pragma pack(push,1)
    struct PavoGroup
    {
        uint16_t id;      // 0xFF01
        uint16_t angle;
        uint16_t dist1;
        uint8_t  inten1;
        uint16_t dist2;
        uint8_t  inten2;
    };
#pragma pack(pop)
};
