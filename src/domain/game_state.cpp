#include "domain/game_state.h"

#include <algorithm>
#include <random>

namespace domain
{

namespace
{

class DefaultRandom : public GameState::Random
{
public:
    int next(int bound) override
    {
        if (bound <= 0)
        {
            return 0;
        }
        std::uniform_int_distribution<int> distribution(0, bound - 1);
        return distribution(m_engine);
    }

private:
    std::mt19937 m_engine{std::random_device{}()};
};

} // namespace

GameState::GameState()
    : m_random(nullptr)
{
    reset();
}

GameState::GameState(Random* random)
    : m_random(random)
{
    reset();
}

GameState::Random* GameState::randomSource()
{
    if (m_random != nullptr)
    {
        return m_random;
    }
    static DefaultRandom fallback;
    return &fallback;
}

void GameState::reset()
{
    for (int i = 0; i < g_node_count; ++i)
    {
        m_board[static_cast<std::size_t>(i)] = Graph::solvedValueAt(i);
    }
    m_history.clear();
    m_moveCount = 0;
}

int GameState::blankIndex() const
{
    for (int i = 0; i < g_node_count; ++i)
    {
        if (m_board[static_cast<std::size_t>(i)] == 0)
        {
            return i;
        }
    }
    return -1;
}

void GameState::scramble(Difficulty level)
{
    reset();

    const int steps = scrambleSteps(level);
    int previousBlank = -1;
    for (int step = 0; step < steps; ++step)
    {
        const int blank = blankIndex();
        if (blank < 0)
        {
            break;
        }

        std::vector<int> candidates;
        candidates.reserve(m_graph.degree(blank));
        for (int neighbor : m_graph.neighbors(blank))
        {
            if (neighbor != previousBlank)
            {
                candidates.push_back(neighbor);
            }
        }
        if (candidates.empty())
        {
            for (int neighbor : m_graph.neighbors(blank))
            {
                candidates.push_back(neighbor);
            }
        }
        if (candidates.empty())
        {
            break;
        }

        const int pick =
            candidates.size() == 1
                ? candidates.front()
                : candidates[static_cast<std::size_t>(randomSource()->next(
                      static_cast<int>(candidates.size())))];
        std::swap(m_board[static_cast<std::size_t>(blank)],
                  m_board[static_cast<std::size_t>(pick)]);
        previousBlank = blank;
    }

    m_history.clear();
    m_moveCount = 0;
}

bool GameState::isMovable(int index) const
{
    if (index < 0 || index >= g_node_count)
    {
        return false;
    }
    if (m_board[static_cast<std::size_t>(index)] == 0)
    {
        return false;
    }
    const int blank = blankIndex();
    return blank >= 0 && m_graph.areAdjacent(index, blank);
}

bool GameState::move(int index)
{
    if (!isMovable(index))
    {
        return false;
    }

    const int blank = blankIndex();
    std::swap(m_board[static_cast<std::size_t>(index)],
              m_board[static_cast<std::size_t>(blank)]);
    ++m_moveCount;
    m_history.push_back(blank);
    return true;
}

bool GameState::undo()
{
    if (m_history.empty())
    {
        return false;
    }

    const int from = m_history.back();
    m_history.pop_back();
    const int blank = blankIndex();
    if (from < 0 || blank < 0)
    {
        return false;
    }

    std::swap(m_board[static_cast<std::size_t>(from)],
              m_board[static_cast<std::size_t>(blank)]);
    if (m_moveCount > 0)
    {
        --m_moveCount;
    }
    return true;
}

bool GameState::isSolved() const
{
    for (int i = 0; i < g_node_count; ++i)
    {
        if (m_board[static_cast<std::size_t>(i)] != Graph::solvedValueAt(i))
        {
            return false;
        }
    }
    return true;
}

int GameState::valueAt(int index) const
{
    if (index < 0 || index >= g_node_count)
    {
        return -1;
    }
    return m_board[static_cast<std::size_t>(index)];
}

std::vector<int> GameState::board() const
{
    return std::vector<int>(m_board.begin(), m_board.end());
}

bool GameState::isValidBoard(const std::array<int, g_node_count>& board)
{
    std::array<bool, g_node_count> seen{};
    for (const int value : board)
    {
        if (value < 0 || value >= g_node_count || seen[static_cast<std::size_t>(value)])
        {
            return false;
        }
        seen[static_cast<std::size_t>(value)] = true;
    }
    return true;
}

bool GameState::restore(const std::vector<int>& snapshot)
{
    if (snapshot.size() != static_cast<std::size_t>(g_node_count))
    {
        return false;
    }

    std::array<int, g_node_count> next{};
    std::copy(snapshot.begin(), snapshot.end(), next.begin());
    if (!isValidBoard(next))
    {
        return false;
    }

    m_board = next;
    m_history.clear();
    m_moveCount = 0;
    return true;
}

} // namespace domain
