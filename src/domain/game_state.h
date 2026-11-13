#pragma once

/**
 * @file game_state.h
 * @brief 棋盘状态、移动规则、悔棋历史与打乱。
 */

#include <array>
#include <cstdint>
#include <vector>

#include "domain/difficulty.h"
#include "domain/graph.h"

namespace domain
{

/**
 * @brief 纯游戏状态与规则。
 *
 * 棋盘是 16 个取值的数组，0 表示空格。打乱从已完成局面出发做合法的随机
 * 游走，因此生成的每个局面都可解，不需要随机置换滑块谜题所用的奇偶性校验。
 */
class GameState
{
  public:
    /// @brief 可注入的随机源，让测试能拿到确定的结果。
    class Random
    {
      public:
        /// @brief 允许通过基类指针销毁派生的随机源。
        virtual ~Random() = default;

        /**
         * @brief 返回 [0, bound) 内的均匀随机整数。
         * @param bound 开区间上界，必须为正数。
         */
        virtual int next(int bound) = 0;
    };

    /// @brief 用内置随机源构造已完成局面。
    GameState();

    /**
     * @brief 用外部随机源构造已完成局面。
     * @param random 随机源；传 nullptr 时回落到内置随机源。
     */
    explicit GameState(Random* random);

    /**
     * @brief 打乱成新的一局。
     * @param level 难度等级，决定合法移动的步数。
     * @note 打乱会一并清空悔棋历史并把步数归零。
     */
    void scramble(Difficulty level);

    /**
     * @brief 把一个节点移入空格。
     * @param index 节点下标。
     * @return 移动合法并已执行时为 true。
     * @note 成功移动会把交换前的空格位置压入悔棋历史，并把步数加一。
     */
    bool move(int index);

    /**
     * @brief 撤销最近一次移动。
     * @return 存在可撤销的一步且已撤销时为 true。
     * @note 历史为空时状态保持不变；撤销成功会把步数减一。
     */
    bool undo();

    /// @brief 是否还有可撤销的移动。
    /// @return 悔棋历史非空时为 true。
    bool canUndo() const
    {
        return !m_history.empty();
    }

    /// @brief 是否已完成。
    /// @return 棋盘与已完成布局逐节点一致时为 true。
    bool isSolved() const;

    /// @brief 空格所在的节点。
    /// @return 空格下标；棋盘里找不到 0 时为 -1。
    int blankIndex() const;

    /**
     * @brief 取一个节点的取值。
     * @param index 节点下标。
     * @return 该节点的取值；下标非法时为 -1。
     */
    int valueAt(int index) const;

    /**
     * @brief 判断一个节点能否移入空格。
     * @param index 节点下标。
     * @return 该节点非空且与空格相邻时为 true。
     */
    bool isMovable(int index) const;

    /// @brief 取步数。
    /// @return 上一次打乱或恢复以来的玩家移动步数。
    int moveCount() const
    {
        return m_moveCount;
    }

    /// @brief 取棋盘拓扑。
    /// @return 构造完成即固定、不随游戏进程变化的连接关系。
    const Graph& graph() const
    {
        return m_graph;
    }

    /// @brief 取全部节点取值的快照。
    /// @return 长度 g_node_count 的取值数组，0 表示空格。
    std::vector<int> board() const;

    /**
     * @brief 恢复一份保存过的棋盘快照。
     * @param snapshot 棋盘取值，必须是 0..15 的一个排列。
     * @return 快照非法时为 false，此时状态保持不变。
     * @note 恢复成功会清空悔棋历史并把步数归零。
     */
    bool restore(const std::vector<int>& snapshot);

  private:
    /// @brief 回到已完成局面并清空历史与步数。
    void reset();

    /// @brief 取实际使用的随机源。
    ///
    /// @return 构造时注入的随机源；未注入时返回进程内共享的内置随机源。
    Random* randomSource();

    /**
     * @brief 校验一组取值是否构成合法棋盘。
     *
     * 合法棋盘是 0..g_node_count-1 各出现一次的排列。
     *
     * @param board 待校验的取值数组，长度固定为 @c g_node_count。
     */
    static bool isValidBoard(const std::array<int, g_node_count>& board);

    Graph m_graph;                           ///< 固定拓扑，构造后保持不变。
    std::array<int, g_node_count> m_board{}; ///< 每个节点的取值；0 表示空格。
    std::vector<int> m_history;              ///< 悔棋历史；每项记录一次移动前的空格下标。
    int m_moveCount = 0;                     ///< 上一次打乱或恢复以来的玩家步数。
    Random* m_random = nullptr; ///< 注入的随机源；为空时使用进程内共享的内置随机源。
};

} // namespace domain
