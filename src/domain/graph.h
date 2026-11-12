#pragma once

/**
 * @file graph.h
 * @brief 不可变的棋盘连接拓扑。
 */

#include <array>
#include <cstddef>
#include <vector>

namespace domain
{

/// @brief 棋盘边长（4x4）。
constexpr int g_board_size = 4;

/// @brief 节点总数。
constexpr int g_node_count = g_board_size * g_board_size;

/**
 * @brief 棋盘连接拓扑。
 *
 * 节点按 4x4 方阵摆放，但边不是网格相邻关系：全图共 22 条边，其中包含
 * 1-6、3-8 这类对角连接。该类型只描述连接关系，不持有任何游戏状态。
 */
class Graph
{
public:
    /// @brief 构建完整拓扑。
    Graph();

    /**
     * @brief 取一个节点的邻居。
     * @param index 节点下标，取值 [0, g_node_count)。
     * @return 只读邻居列表；下标非法时为空表。
     */
    const std::vector<int>& neighbors(int index) const;

    /**
     * @brief 判断两个节点之间是否有边。
     * @param a 第一个节点下标。
     * @param b 第二个节点下标。
     * @return 直接相连时为 true。
     */
    bool areAdjacent(int a, int b) const;

    /**
     * @brief 取一个节点的度。
     * @param index 节点下标。
     * @return 邻居个数；下标非法时为 0。
     */
    std::size_t degree(int index) const;

    /// @brief 取边总数。
    /// @return 内置拓扑的边数，等于 22。
    std::size_t edgeCount() const
    {
        return m_edgeCount;
    }

    /**
     * @brief 节点所在行。
     * @param index 节点下标。
     * @return 行号，取值 [0, 4)；下标非法时为 -1。
     */
    static int rowOf(int index);

    /**
     * @brief 节点所在列。
     * @param index 节点下标。
     * @return 列号，取值 [0, 4)；下标非法时为 -1。
     */
    static int colOf(int index);

    /**
     * @brief 已完成局面下某个节点的取值。
     * @param index 节点下标。
     * @return 普通节点为 1..15，右下角空格为 0，下标非法时为 -1。
     */
    static int solvedValueAt(int index);

private:
    /**
     * @brief 添加一条无向边。
     *
     * 端点以 1 起始编号传入；越界、自环和重复边都被忽略。
     *
     * @param a 第一个端点编号，取值 [1, g_node_count]。
     * @param b 第二个端点编号，取值 [1, g_node_count]。
     */
    void addEdge(int a, int b);

    std::array<std::vector<int>, g_node_count> m_adjacency;
    std::size_t m_edgeCount = 0;
};

} // namespace domain
