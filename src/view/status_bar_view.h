#pragma once

/**
 * @file status_bar_view.h
 * @brief QLabel/status-bar implementation of StatusSink.
 */

#include "view/interfaces.h"

class QAction;
class QLabel;
class QMainWindow;

namespace view
{

/// @brief Formats status and title text for the application shell.
class StatusBarView : public StatusSink
{
public:
    /// @param window Window whose title is updated.
    /// @param difficulty Difficulty label.
    /// @param moves Move-count label.
    /// @param time Elapsed-time label.
    /// @param best Best-record label.
    /// @param undo Undo action whose enabled state follows the model.
    StatusBarView(QMainWindow* window,
                  QLabel* difficulty,
                  QLabel* moves,
                  QLabel* time,
                  QLabel* best,
                  QAction* undo);

    void applyStatus(const StatusState& state) override;
    void setWindowTitle(const QString& title) override;

private:
    QMainWindow* m_window = nullptr;
    QLabel* m_difficulty = nullptr;
    QLabel* m_moves = nullptr;
    QLabel* m_time = nullptr;
    QLabel* m_best = nullptr;
    QAction* m_undo = nullptr;
};

} // namespace view
