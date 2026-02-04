#include "PavoParser.h"

static constexpr float ANGLE_SCALE = 0.01f;   // kiểm tra manual
static constexpr float DIST_SCALE  = 0.001f;  // mm → meter
static constexpr float ANGLE_STEP  = 0.25f;   // ví dụ, chỉnh theo model

bool PavoParser::parse(const uint8_t* data,
                       size_t len,
                       std::vector<LidarPoint>& outPoints)
{
    if (len != PACKET_SIZE)
        return false;

    outPoints.clear();
    outPoints.reserve(GROUP_COUNT * 2);

    for (int g = 0; g < GROUP_COUNT; ++g)
    {
        int offset = g * GROUP_SIZE;
        const PavoGroup* grp =
            reinterpret_cast<const PavoGroup*>(data + offset);

        // Validate ID
        if (grp->id != 0xFF01 && grp->id != 0x01FF)
            continue;

        float baseAngle = grp->angle * ANGLE_SCALE;

        // Point 1
        if (grp->dist1 > 0)
        {
            outPoints.push_back({
                baseAngle,
                grp->dist1 * DIST_SCALE,
                grp->inten1
            });
        }

        // Point 2
        if (grp->dist2 > 0)
        {
            outPoints.push_back({
                baseAngle + ANGLE_STEP,
                grp->dist2 * DIST_SCALE,
                grp->inten2
            });
        }
    }

    return true;
}
