#include "view/difficulty_catalog.h"

#include <QCoreApplication>

namespace view
{

const std::array<domain::Difficulty, domain::kDifficultyCount>& DifficultyCatalog::all() const
{
    static const std::array<domain::Difficulty, domain::kDifficultyCount> levels = domain::allDifficulties();
    return levels;
}

QString DifficultyCatalog::name(domain::Difficulty level) const
{
    switch (level)
    {
    case domain::Difficulty::Easy:
        return QCoreApplication::translate("Difficulty", "简单");
    case domain::Difficulty::Normal:
        return QCoreApplication::translate("Difficulty", "普通");
    case domain::Difficulty::Hard:
        return QCoreApplication::translate("Difficulty", "困难");
    case domain::Difficulty::Expert:
        return QCoreApplication::translate("Difficulty", "专家");
    }
    return QCoreApplication::translate("Difficulty", "普通");
}

} // namespace view
