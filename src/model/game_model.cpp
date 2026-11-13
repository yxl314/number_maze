#include "model/game_model.h"

#include <QTimer>

#include <algorithm>

namespace model
{

namespace
{

constexpr int g_tick_interval_ms = 1000;

} // namespace

GameModel::GameModel(QObject* parent) : QObject(parent), m_timer(new QTimer(this))
{
    m_timer->setInterval(g_tick_interval_ms);
    connect(m_timer, &QTimer::timeout, this, &GameModel::onTick);
}

int GameModel::levelIndex(domain::Difficulty level) const
{
    return domain::difficultyIndex(level);
}

int GameModel::bestRecord(domain::Difficulty level) const
{
    const int value = m_bestRecords[static_cast<std::size_t>(levelIndex(level))];
    return value > 0 ? value : 0;
}

void GameModel::setBestRecords(const std::array<int, domain::kDifficultyCount>& records)
{
    for (std::size_t i = 0; i < m_bestRecords.size(); ++i)
    {
        m_bestRecords[i] = records[i] > 0 ? records[i] : 0;
    }
}

QVector<int> GameModel::boardData() const
{
    const std::vector<int> raw = m_state.board();
    QVector<int> out;
    out.reserve(static_cast<int>(raw.size()));
    for (const int value : raw)
    {
        out.append(value);
    }
    return out;
}

QVector<QVector<int>> GameModel::connectionData() const
{
    const domain::Graph& graph = m_state.graph();
    QVector<QVector<int>> out;
    out.reserve(domain::g_node_count);
    for (int i = 0; i < domain::g_node_count; ++i)
    {
        QVector<int> neighbors;
        neighbors.reserve(static_cast<int>(graph.degree(i)));
        for (const int node : graph.neighbors(i))
        {
            neighbors.append(node);
        }
        out.append(neighbors);
    }
    return out;
}

QVector<bool> GameModel::movableData() const
{
    QVector<bool> out;
    out.reserve(domain::g_node_count);
    for (int i = 0; i < domain::g_node_count; ++i)
    {
        out.append(m_state.isMovable(i));
    }
    return out;
}

void GameModel::startNewGame(domain::Difficulty level)
{
    // 非法难度统一按 Normal 处理，避免界面文案、打乱步数和纪录下标三者不一致。
    m_difficulty = (levelIndex(level) == static_cast<int>(level)) ? level : domain::Difficulty::Normal;
    m_elapsedSeconds = 0;
    m_solvedNotified = false;
    stopTimer();

    m_state.scramble(m_difficulty);
    m_initialBoard = m_state.board();

    emit boardChanged();
    emit statusChanged();
}

void GameModel::restartCurrentGame()
{
    m_elapsedSeconds = 0;
    m_solvedNotified = false;
    stopTimer();

    if (!m_state.restore(m_initialBoard))
    {
        m_state.scramble(m_difficulty);
        m_initialBoard = m_state.board();
    }

    emit boardChanged();
    emit statusChanged();
}

void GameModel::requestMove(int index)
{
    if (m_solvedNotified || !m_state.move(index))
    {
        return;
    }

    startTimer();
    emit boardChanged();

    if (m_state.isSolved())
    {
        stopTimer();
        m_solvedNotified = true;

        const int moves = m_state.moveCount();
        const int previous = bestRecord(m_difficulty);
        const bool isNewRecord = previous <= 0 || moves < previous;
        if (isNewRecord)
        {
            m_bestRecords[static_cast<std::size_t>(levelIndex(m_difficulty))] = moves;
        }

        emit statusChanged();
        emit solved(moves, m_elapsedSeconds, isNewRecord);
        return;
    }

    emit statusChanged();
}

void GameModel::requestUndo()
{
    if (!m_state.undo())
    {
        return;
    }

    m_solvedNotified = false;
    emit boardChanged();
    emit statusChanged();
}

void GameModel::startTimer()
{
    if (m_running)
    {
        return;
    }
    m_running = true;
    m_timer->start();
}

void GameModel::stopTimer()
{
    m_running = false;
    if (m_timer)
    {
        m_timer->stop();
    }
}

void GameModel::onTick()
{
    ++m_elapsedSeconds;
    emit tick();
    emit statusChanged();
}

} // namespace model
