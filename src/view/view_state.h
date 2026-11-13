#pragma once

/**
 * @file view_state.h
 * @brief 控制器推给视图的完整棋盘与状态取值。
 */

#include <QString>
#include <QVector>

namespace view
{

/// @brief 一帧完整的棋盘数据。
struct BoardFrame
{
    QVector<int> board;    ///< 节点取值；0 表示空格。
    QVector<bool> movable; ///< 每个节点的可移动标记。
    bool solved = false;   ///< 是否已完成。
    int solvedMoves = 0;   ///< 完成遮罩中显示的步数。
};

/// @brief 一份完整的状态栏状态。
struct StatusState
{
    QString difficulty;   ///< 本地化的难度名称。
    int moves = 0;        ///< 玩家步数。
    int seconds = 0;      ///< 已用秒数。
    int bestMoves = 0;    ///< 该难度的最佳步数；0 表示没有纪录。
    bool canUndo = false; ///< 是否可以悔棋。
};

/// @brief 通关对话框中展示的数据。
struct SolvedInfo
{
    QString difficulty;     ///< 本地化的难度名称。
    int moves = 0;          ///< 步数。
    int seconds = 0;        ///< 已用秒数。
    bool newRecord = false; ///< 是否刷新了最佳步数。
};

} // namespace view
