#pragma once

/**
 * @file difficulty.h
 * @brief 难度等级定义及其打乱步数预算。
 */

#include <array>
#include <cstdint>

namespace domain
{

/// @brief 支持的难度等级个数。
constexpr int kDifficultyCount = 4;

/**
 * @brief 游戏难度。
 *
 * 取值从 0 起连续排布，可直接作为按难度索引的数组下标。
 */
enum class Difficulty : std::uint8_t
{
    Easy = 0,
    Normal = 1,
    Hard = 2,
    Expert = 3
};

/**
 * @brief 一种难度对应的打乱步数。
 *
 * 步数按合法移动计数，等级越高数值越大。
 *
 * @param level 难度等级。
 * @return 打乱所用的合法移动步数；未知取值按普通难度的 40 步处理。
 */
/// @brief 把任意难度值规范化为合法下标。
///
/// 非法枚举值统一按 Normal 处理；界面文案、打乱步数和纪录下标都使用同一规则。
///
/// @param level 难度等级。
/// @return 0..kDifficultyCount-1 的下标。
inline int difficultyIndex(Difficulty level) noexcept
{
    const int index = static_cast<int>(level);
    return index >= 0 && index < kDifficultyCount ? index : 1;
}

/// @brief 按枚举顺序返回全部难度。
///
/// @return 4 个难度的固定数组。
inline std::array<Difficulty, kDifficultyCount> allDifficulties() noexcept
{
    return {Difficulty::Easy, Difficulty::Normal, Difficulty::Hard, Difficulty::Expert};
}

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
