#pragma once
#include <array>

namespace GravityTable {
    constexpr float frameDuration = 1.0f / 60.0f;

    constexpr std::array<float, 20> framesPerField = 
    {
        48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2
    };

    inline float getGravity(int level) 
    {
        size_t index = static_cast<size_t>((level < 1) ? 0 : level - 1);
        if (index >= framesPerField.size())
            index = framesPerField.size() - 1;

        return framesPerField[index] * frameDuration;
    }
}
