#pragma once

/**
 * @file game_state.h
 * @brief Board state, move rules, undo history and scramble.
 */

#include <array>
#include <cstdint>
#include <vector>

#include "domain/difficulty.h"
#include "domain/graph.h"

namespace domain
{

/**
 * @brief Pure game state and rules.
 *
 * The board is an array of 16 values; 0 is the blank. Scrambling is performed
 * by a legal random walk from the solved state, so every generated position is
 * solvable without the parity test used by randomly permuted sliding puzzles.
 */
class GameState
{
public:
    /// @brief Injectable random source for deterministic tests.
    class Random
    {
    public:
        virtual ~Random() = default;

        /**
         * @brief Uniform integer in [0, bound).
         * @param bound Exclusive upper bound; must be positive.
         */
        virtual int next(int bound) = 0;
    };

    /// @brief Construct a solved board using the built-in random source.
    GameState();

    /**
     * @brief Construct a solved board with an external random source.
     * @param random Random source; nullptr selects the built-in source.
     */
    explicit GameState(Random* random);

    /**
     * @brief Scramble into a new game.
     * @param level Difficulty level; determines the number of legal moves.
     */
    void scramble(Difficulty level);

    /**
     * @brief Move one node into the blank.
     * @param index Node index.
     * @return True when the move was legal and applied.
     */
    bool move(int index);

    /// @return True when an undo entry existed and was applied.
    bool undo();

    /// @return True when any move can be undone.
    bool canUndo() const
    {
        return !m_history.empty();
    }

    /// @return True when the board equals the solved layout.
    bool isSolved() const;

    /// @return Blank index, or -1 when the board is malformed.
    int blankIndex() const;

    /**
     * @brief Value at one node.
     * @param index Node index.
     * @return Stored value, or -1 when the index is invalid.
     */
    int valueAt(int index) const;

    /**
     * @brief Whether a node can move into the blank.
     * @param index Node index.
     */
    bool isMovable(int index) const;

    /// @return Number of player moves since the last scramble/restore.
    int moveCount() const
    {
        return m_moveCount;
    }

    /// @return Board topology.
    const Graph& graph() const
    {
        return m_graph;
    }

    /// @return Snapshot of all node values.
    std::vector<int> board() const;

    /**
     * @brief Restore a previously saved snapshot.
     * @param snapshot Board values; must be a valid permutation of 0..15.
     * @return False when the snapshot is invalid; state is unchanged then.
     */
    bool restore(const std::vector<int>& snapshot);

private:
    void reset();
    Random* randomSource();
    static bool isValidBoard(const std::array<int, g_node_count>& board);

    Graph m_graph;
    std::array<int, g_node_count> m_board{};
    std::vector<int> m_history;
    int m_moveCount = 0;
    Random* m_random = nullptr;
};

} // namespace domain
