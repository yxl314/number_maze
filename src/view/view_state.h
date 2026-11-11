#pragma once

/**
 * @file view_state.h
 * @brief Atomic board/status values pushed by the controller.
 */

#include <QString>
#include <QVector>

namespace view
{

/// @brief One complete board frame.
struct BoardFrame
{
    QVector<int> board;      ///< Node values; 0 is the blank.
    QVector<bool> movable;   ///< Per-node movable flags.
    bool solved = false;     ///< Whether the puzzle is solved.
    int solvedMoves = 0;     ///< Move count shown in the solved overlay.
};

/// @brief One complete status-bar state.
struct StatusState
{
    QString difficulty; ///< Localized difficulty name.
    int moves = 0;      ///< Player move count.
    int seconds = 0;    ///< Elapsed seconds.
    int bestMoves = 0;  ///< Best move count for this difficulty; 0 = none.
    bool canUndo = false; ///< Whether undo is available.
};

/// @brief Data shown in the solved dialog.
struct SolvedInfo
{
    QString difficulty; ///< Localized difficulty name.
    int moves = 0;      ///< Move count.
    int seconds = 0;    ///< Elapsed seconds.
    bool newRecord = false; ///< Whether this run set a new best.
};

} // namespace view
