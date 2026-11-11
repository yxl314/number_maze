#include "service/record_store.h"

#include <QSettings>
#include <QString>

#include <utility>

namespace service
{

namespace
{

QString recordKey(domain::Difficulty level)
{
    return QStringLiteral("records/%1").arg(static_cast<int>(level));
}

} // namespace

RecordStore::RecordStore(std::unique_ptr<QSettings> settings)
    : m_settings(std::move(settings))
{
}

RecordStore::~RecordStore() = default;

int RecordStore::levelIndex(domain::Difficulty level)
{
    const int index = static_cast<int>(level);
    return index >= 0 && index < domain::kDifficultyCount ? index : 0;
}

int RecordStore::bestMoves(domain::Difficulty level) const
{
    if (!m_settings)
    {
        return 0;
    }
    const int value =
        m_settings->value(recordKey(domain::Difficulty(levelIndex(level))), 0).toInt();
    return value > 0 ? value : 0;
}

void RecordStore::saveBestMoves(domain::Difficulty level, int moves)
{
    if (!m_settings || moves <= 0)
    {
        return;
    }
    m_settings->setValue(recordKey(domain::Difficulty(levelIndex(level))), moves);
    m_settings->sync();
}

} // namespace service
