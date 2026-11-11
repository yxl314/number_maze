#include "controller/game_controller.h"

#include <QString>

#include <array>

#include "model/game_model.h"
#include "service/record_store.h"
#include "view/board_view.h"
#include "view/difficulty_catalog.h"
#include "view/interfaces.h"
#include "view/view_state.h"

namespace controller
{

GameController::GameController(view::BoardView* view,
                               model::GameModel* model,
                               view::StatusSink* status,
                               view::SolvedSink* solved,
                               service::RecordStore* records,
                               const view::DifficultyCatalog* catalog,
                               QObject* parent)
    : QObject(parent),
      m_view(view),
      m_model(model),
      m_status(status),
      m_solved(solved),
      m_records(records),
      m_catalog(catalog)
{
}

void GameController::initialize()
{
    loadRecords();

    connect(m_view, &view::BoardView::nodeClicked, this, &GameController::handleNodeClicked);
    connect(m_model, &model::GameModel::boardChanged, this, &GameController::handleBoardChanged);
    connect(m_model, &model::GameModel::statusChanged, this, &GameController::handleStatusChanged);
    connect(m_model, &model::GameModel::solved, this, &GameController::handleSolved);

    m_view->setConnections(m_model->connectionData());
    m_model->startNewGame(domain::Difficulty::Normal);
    syncBoardToView();
    refreshStatus();
}

void GameController::loadRecords()
{
    if (!m_records || !m_model)
    {
        return;
    }

    const std::array<domain::Difficulty, domain::kDifficultyCount> levels = {
        domain::Difficulty::Easy,
        domain::Difficulty::Normal,
        domain::Difficulty::Hard,
        domain::Difficulty::Expert};
    std::array<int, domain::kDifficultyCount> records{};
    for (std::size_t i = 0; i < levels.size(); ++i)
    {
        records[i] = m_records->bestMoves(levels[i]);
    }
    m_model->setBestRecords(records);
}

void GameController::handleNodeClicked(int index)
{
    m_model->requestMove(index);
}

void GameController::handleBoardChanged()
{
    syncBoardToView();
}

void GameController::handleStatusChanged()
{
    refreshStatus();
}

void GameController::handleSolved(int moves, int seconds, bool newRecord)
{
    if (newRecord && m_records)
    {
        m_records->saveBestMoves(m_model->difficulty(), moves);
    }

    refreshStatus();
    if (m_solved)
    {
        const QString name = m_catalog ? m_catalog->name(m_model->difficulty()) : QString();
        m_solved->showSolved(view::SolvedInfo{name, moves, seconds, newRecord});
    }
}

void GameController::handleNewGame(domain::Difficulty level)
{
    m_model->startNewGame(level);
}

void GameController::handleRestart()
{
    m_model->restartCurrentGame();
}

void GameController::handleUndo()
{
    m_model->requestUndo();
}

void GameController::syncBoardToView()
{
    view::BoardFrame frame;
    frame.board = m_model->boardData();
    frame.movable = m_model->movableData();
    frame.solved = m_model->isSolved();
    frame.solvedMoves = m_model->moveCount();
    m_view->applyFrame(frame);
}

void GameController::refreshStatus()
{
    if (!m_status)
    {
        return;
    }

    view::StatusState state;
    state.difficulty = m_catalog ? m_catalog->name(m_model->difficulty()) : QString();
    state.moves = m_model->moveCount();
    state.seconds = m_model->elapsedSeconds();
    state.bestMoves = m_model->bestRecord(m_model->difficulty());
    state.canUndo = m_model->canUndo();
    m_status->applyStatus(state);
    m_status->setWindowTitle(QStringLiteral("数字迷宫 - %1").arg(state.difficulty));
}

} // namespace controller
