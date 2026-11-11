#pragma once

/**
 * @file difficulty.h
 * @brief Game difficulty levels and their scramble budgets.
 */

#include <cstdint>

namespace domain
{

/// @brief Number of supported difficulty levels.
constexpr int kDifficultyCount = 4;

/// @brief Game difficulty.
enum class Difficulty : std::uint8_t
{
    Easy = 0,
    Normal = 1,
    Hard = 2,
    Expert = 3
};

/**
 * @brief Scramble steps for one difficulty.
 * @param level Difficulty level.
 * @return Number of legal moves used to scramble.
 */
inline int scrambleSteps(Difficulty level)
{
    switch (level)
    {
    case Difficulty::Easy:
        return 20;
    case Difficulty::Normal:
        return 40;
    case Difficulty::Hard:
        return 80;
    case Difficulty::Expert:
        return 160;
    }
    return 40;
}

} // namespace domain
