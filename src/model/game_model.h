#pragma once

/**
 * @file game_model.h
 * @brief Observable game model: state, timer and in-memory records.
 */

#include <QObject>
#include <QTime>
#include <QVector>

#include <array>
#include <vector>

#include "domain/difficulty.h"
#include "domain/game_state.h"

class QTimer;

namespace model
{

/**
 * @brief Observable application model for the number maze.
 *
 * The model knows nothing about views, controllers or persistence. Game rules
 * are delegated to domain::GameState; best scores are held in memory and
 * loaded/saved by the controller through service::RecordStore.
 */
class GameModel : public QObject
{
    Q_OBJECT

public:
    explicit GameModel(QObject* parent = nullptr);

    /// @return Current difficulty.
    domain::Difficulty difficulty() const
    {
        return m_difficulty;
    }

    /// @return Player move count since the last scramble/restart.
    int moveCount() const
    {
        return m_state.moveCount();
    }

    /// @return Elapsed seconds.
    int elapsedSeconds() const
    {
        return m_elapsedSeconds;
    }

    /// @return Elapsed time as QTime for formatting.
    QTime elapsed() const
    {
        return QTime(0, 0).addSecs(m_elapsedSeconds);
    }

    /// @return True when an undo is available.
    bool canUndo() const
    {
        return m_state.canUndo();
    }

    /// @return True when the board equals the solved layout.
    bool isSolved() const
    {
        return m_state.isSolved();
    }

    /**
     * @brief Best move count for a difficulty.
     * @param level Difficulty level.
     * @return Best moves, or 0 when no record exists.
     */
    int bestRecord(domain::Difficulty level) const;

    /// @return All in-memory best records indexed by difficulty.
    const std::array<int, domain::kDifficultyCount>& bestRecords() const
    {
        return m_bestRecords;
    }

    /**
     * @brief Replace the in-memory best records.
     * @param records Record values indexed by difficulty.
     */
    void setBestRecords(const std::array<int, domain::kDifficultyCount>& records);

    /// @return Current board values; 0 is the blank.
    QVector<int> boardData() const;

    /// @return Static adjacency table for the view.
    QVector<QVector<int>> connectionData() const;

    /// @return Per-node movable flags for the view.
    QVector<bool> movableData() const;

public slots:
    /// @param level Difficulty level.
    void startNewGame(domain::Difficulty level);

    /// @brief Start a new game with the current difficulty.
    void startNewGame();

    /// @brief Restore the current game's initial position and reset counters.
    void restartCurrentGame();

    /// @param index Node index requested by the user.
    void requestMove(int index);

    /// @brief Undo the last move.
    void requestUndo();

signals:
    void boardChanged();
    void statusChanged();
    void tick();
    void solved(int moves, int seconds, bool isNewRecord);

private slots:
    void onTick();

private:
    void startTimer();
    void stopTimer();
    int levelIndex(domain::Difficulty level) const;

    domain::GameState m_state;
    std::vector<int> m_initialBoard;
    domain::Difficulty m_difficulty = domain::Difficulty::Normal;
    QTimer* m_timer = nullptr;
    int m_elapsedSeconds = 0;
    bool m_running = false;
    bool m_solvedNotified = false;
    std::array<int, domain::kDifficultyCount> m_bestRecords{};
};

} // namespace model
