#pragma once

/**
 * @file game_model.h
 * @brief 可观察的游戏模型：状态、计时与内存中的纪录。
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
 * @brief 数字迷宫的可观察应用模型。
 *
 * 模型不认识视图、控制器和持久化：游戏规则委托给 domain::GameState，
 * 最佳步数只保存在内存里，由控制器通过 service::RecordStore 读写。
 */
class GameModel : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造模型并准备好一秒一跳的计时器。
     * @param parent Qt 对象树的父对象。
     */
    explicit GameModel(QObject* parent = nullptr);

    /// @brief 取当前难度。
    /// @return 本局开始时选定的难度。
    domain::Difficulty difficulty() const
    {
        return m_difficulty;
    }

    /// @brief 取步数。
    /// @return 上一次打乱或重开以来的玩家移动步数。
    int moveCount() const
    {
        return m_state.moveCount();
    }

    /// @brief 取已用秒数。
    /// @return 从本局第一次移动开始累计的秒数。
    int elapsedSeconds() const
    {
        return m_elapsedSeconds;
    }

    /// @brief 取已用时长。
    /// @return 供界面按 hh:mm:ss 格式化的 QTime。
    QTime elapsed() const
    {
        return QTime(0, 0).addSecs(m_elapsedSeconds);
    }

    /// @brief 是否还有可撤销的移动。
    /// @return 存在悔棋历史时为 true。
    bool canUndo() const
    {
        return m_state.canUndo();
    }

    /// @brief 是否已完成。
    /// @return 棋盘与已完成布局一致时为 true。
    bool isSolved() const
    {
        return m_state.isSolved();
    }

    /**
     * @brief 取某个难度的最佳步数。
     * @param level 难度等级。
     * @return 最佳步数；没有纪录时为 0。
     */
    int bestRecord(domain::Difficulty level) const;

    /// @brief 取全部最佳步数。
    /// @return 按难度索引的内存纪录数组。
    const std::array<int, domain::kDifficultyCount>& bestRecords() const
    {
        return m_bestRecords;
    }

    /**
     * @brief 整体写入内存中的最佳步数。
     *
     * 非正数按“没有纪录”处理，存为 0。
     *
     * @param records 按难度索引的纪录数组。
     * @param domain::kDifficultyCount 数组长度，取难度等级总数的编译期常量，不占运行时实参。
     */
    void setBestRecords(const std::array<int, domain::kDifficultyCount>& records);

    /// @brief 供视图使用的棋盘取值。
    /// @return 节点取值数组，0 表示空格。
    QVector<int> boardData() const;

    /// @brief 供视图使用的静态邻接表。
    /// @return 每个节点一项的邻居下标表。
    QVector<QVector<int>> connectionData() const;

    /// @brief 供视图使用的可移动标记。
    /// @return 每个节点一项，表示该节点当前能否移入空格。
    QVector<bool> movableData() const;

public slots:
    /**
     * @brief 按指定难度开始新的一局。
     * @param level 难度等级。
     * @note 会重置计时与完成标记，并把打乱后的局面保存为本局的初始局面。
     */
    void startNewGame(domain::Difficulty level);

    /// @brief 按当前难度重新开始一局。
    void startNewGame();

    /**
     * @brief 把当前一局恢复到初始局面，并清零计时与步数。
     * @note 初始局面无法恢复时会重新打乱一局。
     */
    void restartCurrentGame();

    /**
     * @brief 请求把某个节点移入空格。
     * @param index 用户点击的节点下标。
     * @note 本局已判定完成时请求被忽略，非法移动同样直接丢弃。
     */
    void requestMove(int index);

    /// @brief 撤销最近一次移动。
    void requestUndo();

signals:
    /// @brief 棋盘取值或可移动状态发生变化。
    void boardChanged();

    /// @brief 步数、用时或最佳成绩发生变化。
    void statusChanged();

    /// @brief 计时器走过一秒。
    void tick();

    /**
     * @brief 本局完成。
     * @param moves 本局步数。
     * @param seconds 本局用时（秒）。
     * @param isNewRecord 是否刷新了该难度的最佳步数。
     */
    void solved(int moves, int seconds, bool isNewRecord);

private slots:
    /// @brief 计时器每次超时：累加用时并通知界面。
    void onTick();

private:
    /// @brief 启动一秒一跳的计时器；已在运行时不重复启动。
    void startTimer();

    /// @brief 停止计时器。
    void stopTimer();

    /**
     * @brief 难度在纪录数组中的下标。
     * @param level 难度等级。
     * @return 合法等级返回其下标，其余取值返回 0（普通难度）。
     */
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
