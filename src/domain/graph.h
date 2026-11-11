#pragma once

/**
 * @file graph.h
 * @brief Immutable board topology.
 */

#include <array>
#include <cstddef>
#include <vector>

namespace domain
{

/// @brief Board side length (4x4).
constexpr int g_board_size = 4;

/// @brief Total node count.
constexpr int g_node_count = g_board_size * g_board_size;

/**
 * @brief Board connection topology.
 *
 * Nodes are arranged as a 4x4 square, but the edges are not grid neighbors:
 * there are 22 edges including diagonal links such as 1-6 and 3-8. The graph
 * only describes connections; it owns no game state.
 */
class Graph
{
public:
    /// @brief Build the complete topology.
    Graph();

    /**
     * @brief Neighbors of one node.
     * @param index Node index in [0, g_node_count).
     * @return Immutable neighbor list; empty for an invalid index.
     */
    const std::vector<int>& neighbors(int index) const;

    /**
     * @brief Test whether two nodes share an edge.
     * @param a First node index.
     * @param b Second node index.
     * @return True when directly connected.
     */
    bool areAdjacent(int a, int b) const;

    /**
     * @brief Degree of one node.
     * @param index Node index.
     * @return Neighbor count; 0 for an invalid index.
     */
    std::size_t degree(int index) const;

    /// @return Total edge count (22 for the built-in topology).
    std::size_t edgeCount() const
    {
        return m_edgeCount;
    }

    /// @param index Node index.
    /// @return Row in [0, 4); -1 for an invalid index.
    static int rowOf(int index);

    /// @param index Node index.
    /// @return Column in [0, 4); -1 for an invalid index.
    static int colOf(int index);

    /**
     * @brief Value expected at a solved position.
     * @param index Node index.
     * @return 1..15 for normal nodes, 0 for the blank, -1 for invalid index.
     */
    static int solvedValueAt(int index);

private:
    void addEdge(int a, int b);

    std::array<std::vector<int>, g_node_count> m_adjacency;
    std::size_t m_edgeCount = 0;
};

} // namespace domain
