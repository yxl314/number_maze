#pragma once

/**
 * @file difficulty_catalog.h
 * @brief 难度枚举到本地化展示文本的映射。
 */

#include <QString>

#include <array>

#include "domain/difficulty.h"

namespace view
{

/// @brief 难度名称表；领域层不携带任何展示文本。
class DifficultyCatalog
{
public:
    /// @brief 全部难度。
    /// @return 按菜单顺序排列的难度数组。
    const std::array<domain::Difficulty, domain::kDifficultyCount>& all() const;

    /**
     * @brief 难度的本地化名称。
     * @param level 难度等级。
     * @return 对应的中文名称；未知取值返回普通难度的名称。
     */
    QString name(domain::Difficulty level) const;
};

} // namespace view
