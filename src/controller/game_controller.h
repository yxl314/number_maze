#pragma once

/**
 * @file game_controller.h
 * @brief Passive-view MVC controller for the number maze.
 */

#include <QObject>

#include "domain/difficulty.h"

namespace view
{
class BoardView;
class DifficultyCatalog;
class SolvedSink;
class StatusSink;
} // namespace view

namespace model
{
class GameModel;
}

namespace service
{
class RecordStore;
}

namespace controller
{

/**
 * @brief Coordinates the passive board view, model and shell output ports.
 *
 * The controller is the only object that holds both the writable model and
 * concrete view pointers. It contains no game rules and no drawing code.
 */
class GameController : public QObject
{
    Q_OBJECT

public:
    GameController(view::BoardView* view,
                   model::GameModel* model,
                   view::StatusSink* status,
                   view::SolvedSink* solved,
                   service::RecordStore* records,
                   const view::DifficultyCatalog* catalog,
                   QObject* parent = nullptr);

    /// @brief Connect signals, load records and perform the initial sync.
    void initialize();

public slots:
    /// @param level Difficulty selected from the menu.
    void handleNewGame(domain::Difficulty level);

    /// @brief Restart the current game.
    void handleRestart();

    /// @brief Undo the last move.
    void handleUndo();

private slots:
    void handleNodeClicked(int index);
    void handleBoardChanged();
    void handleStatusChanged();
    void handleSolved(int moves, int seconds, bool newRecord);

private:
    void syncBoardToView();
    void refreshStatus();
    void loadRecords();

    view::BoardView* m_view = nullptr;
    model::GameModel* m_model = nullptr;
    view::StatusSink* m_status = nullptr;
    view::SolvedSink* m_solved = nullptr;
    service::RecordStore* m_records = nullptr;
    const view::DifficultyCatalog* m_catalog = nullptr;
};

} // namespace controller
