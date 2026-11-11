#include "domain/graph.h"

#include <algorithm>

namespace domain
{

Graph::Graph()
{
    addEdge(1, 5);
    addEdge(1, 6);
    addEdge(1, 2);
    addEdge(2, 5);
    addEdge(2, 7);
    addEdge(3, 6);
    addEdge(3, 8);
    addEdge(3, 4);
    addEdge(4, 7);
    addEdge(4, 8);
    addEdge(6, 11);
    addEdge(7, 10);
    addEdge(9, 13);
    addEdge(9, 14);
    addEdge(10, 13);
    addEdge(10, 15);
    addEdge(11, 14);
    addEdge(11, 16);
    addEdge(12, 15);
    addEdge(12, 16);
    addEdge(13, 14);
    addEdge(15, 16);
}

void Graph::addEdge(int a, int b)
{
    const int ia = a - 1;
    const int ib = b - 1;
    if (ia < 0 || ia >= g_node_count || ib < 0 || ib >= g_node_count || ia == ib)
    {
        return;
    }

    auto& neighborsA = m_adjacency[static_cast<std::size_t>(ia)];
    if (std::find(neighborsA.begin(), neighborsA.end(), ib) == neighborsA.end())
    {
        neighborsA.push_back(ib);
        m_adjacency[static_cast<std::size_t>(ib)].push_back(ia);
        ++m_edgeCount;
    }
}

const std::vector<int>& Graph::neighbors(int index) const
{
    static const std::vector<int> empty;
    if (index < 0 || index >= g_node_count)
    {
        return empty;
    }
    return m_adjacency[static_cast<std::size_t>(index)];
}

bool Graph::areAdjacent(int a, int b) const
{
    if (a < 0 || a >= g_node_count || b < 0 || b >= g_node_count)
    {
        return false;
    }
    const auto& neighbors = m_adjacency[static_cast<std::size_t>(a)];
    return std::find(neighbors.begin(), neighbors.end(), b) != neighbors.end();
}

std::size_t Graph::degree(int index) const
{
    return neighbors(index).size();
}

int Graph::rowOf(int index)
{
    return index < 0 || index >= g_node_count ? -1 : index / g_board_size;
}

int Graph::colOf(int index)
{
    return index < 0 || index >= g_node_count ? -1 : index % g_board_size;
}

int Graph::solvedValueAt(int index)
{
    if (index < 0 || index >= g_node_count)
    {
        return -1;
    }
    return index == g_node_count - 1 ? 0 : index + 1;
}

} // namespace domain
