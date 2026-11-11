#pragma once

/**
 * @file record_store.h
 * @brief QSettings-backed best-score persistence.
 */

#include <memory>

#include "domain/difficulty.h"

class QSettings;

namespace service
{

/**
 * @brief Persists the best move count for each difficulty.
 *
 * This is the only layer that touches QSettings; the model keeps an in-memory
 * copy so it remains testable without filesystem state.
 */
class RecordStore
{
public:
    /**
     * @param settings Owning QSettings instance; may be null in tests.
     */
    explicit RecordStore(std::unique_ptr<QSettings> settings);
    ~RecordStore();

    RecordStore(const RecordStore&) = delete;
    RecordStore& operator=(const RecordStore&) = delete;

    /// @param level Difficulty level.
    /// @return Best move count, or 0 when no record exists.
    int bestMoves(domain::Difficulty level) const;

    /**
     * @brief Save one new best score.
     * @param level Difficulty level.
     * @param moves Move count.
     */
    void saveBestMoves(domain::Difficulty level, int moves);

private:
    static int levelIndex(domain::Difficulty level);

    std::unique_ptr<QSettings> m_settings;
};

} // namespace service
