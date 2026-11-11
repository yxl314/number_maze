#pragma once

/**
 * @file difficulty_catalog.h
 * @brief Difficulty enum to localized presentation text.
 */

#include <QString>

#include <array>

#include "domain/difficulty.h"

namespace view
{

/// @brief Localized difficulty names; the domain carries no display text.
class DifficultyCatalog
{
public:
    /// @return All difficulties in menu order.
    const std::array<domain::Difficulty, domain::kDifficultyCount>& all() const;

    /// @param level Difficulty level.
    /// @return Localized name, or the normal name for an unknown level.
    QString name(domain::Difficulty level) const;
};

} // namespace view
