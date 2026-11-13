#pragma once

/**
 * @file status_bar_view.h
 * @brief StatusSink 的标签与状态栏实现。
 */

#include "view/interfaces.h"

class QAction;
class QLabel;
class QMainWindow;

namespace view
{

/// @brief 为应用外壳生成状态栏文本与窗口标题。
class StatusBarView : public StatusSink
{
  public:
    /**
     * @brief 保存要更新的控件，此处不写任何文本。
     *
     * 每个指针都可以为空，为空时对应的更新被跳过；控件由外壳持有。
     *
     * @param window 需要更新标题的窗口。
     * @param difficulty 难度标签。
     * @param moves 步数标签。
     * @param time 用时标签。
     * @param best 最佳成绩标签。
     * @param undo 随悔棋可用状态启停的动作。
     */
    StatusBarView(QMainWindow* window, QLabel* difficulty, QLabel* moves, QLabel* time, QLabel* best, QAction* undo);

    /**
     * @brief 应用一份完整的状态栏状态。
     * @param state 难度、步数、用时、最佳成绩与悔棋可用状态。
     */
    void applyStatus(const StatusState& state) override;

    /**
     * @brief 设置窗口标题。
     * @param title 新标题。
     */
    void setWindowTitle(const QString& title) override;

  private:
    QMainWindow* m_window = nullptr; ///< 需要更新标题的窗口，可为空。
    QLabel* m_difficulty = nullptr;  ///< 难度标签，可为空。
    QLabel* m_moves = nullptr;       ///< 步数标签，可为空。
    QLabel* m_time = nullptr;        ///< 用时标签，可为空。
    QLabel* m_best = nullptr;        ///< 最佳成绩标签，可为空。
    QAction* m_undo = nullptr;       ///< 随悔棋可用状态启停的动作，可为空。
};

} // namespace view
