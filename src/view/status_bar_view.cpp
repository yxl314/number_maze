#include "view/status_bar_view.h"

#include <QtWidgets/QAction>
#include <QLabel>
#include <QMainWindow>
#include <QTime>

namespace view
{

StatusBarView::StatusBarView(QMainWindow* window,
                             QLabel* difficulty,
                             QLabel* moves,
                             QLabel* time,
                             QLabel* best,
                             QAction* undo)
    : m_window(window),
      m_difficulty(difficulty),
      m_moves(moves),
      m_time(time),
      m_best(best),
      m_undo(undo)
{
}

void StatusBarView::applyStatus(const StatusState& state)
{
    if (m_difficulty)
    {
        m_difficulty->setText(QStringLiteral("难度：%1").arg(state.difficulty));
    }
    if (m_moves)
    {
        m_moves->setText(QStringLiteral("步数：%1").arg(state.moves));
    }
    if (m_time)
    {
        m_time->setText(QStringLiteral("用时：%1")
                            .arg(QTime(0, 0).addSecs(state.seconds).toString(QStringLiteral("hh:mm:ss"))));
    }
    if (m_best)
    {
        m_best->setText(state.bestMoves > 0
                            ? QStringLiteral("最佳：%1 步").arg(state.bestMoves)
                            : QStringLiteral("最佳：暂无"));
    }
    if (m_undo)
    {
        m_undo->setEnabled(state.canUndo);
    }
}

void StatusBarView::setWindowTitle(const QString& title)
{
    if (m_window)
    {
        m_window->setWindowTitle(title);
    }
}

} // namespace view
