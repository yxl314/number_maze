#pragma once

/**
 * @file main_window.h
 * @brief Application composition root and shell.
 */

#include <QMainWindow>

#include <memory>

class QAction;
class QLabel;

namespace model
{
class GameModel;
}

namespace controller
{
class GameController;
}

namespace service
{
class RecordStore;
}

namespace view
{
class BoardView;
class DifficultyCatalog;
class MessageBoxSolvedSink;
class StatusBarView;
} // namespace view

namespace app
{

/**
 * @brief Composition root for the number maze.
 *
 * Creates the model, passive views, services and controller, then wires menu
 * actions to controller slots. It contains no game rules or signal forwarding.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void createUndoAction();

    view::BoardView* m_view = nullptr;
    model::GameModel* m_model = nullptr;
    controller::GameController* m_controller = nullptr;

    std::unique_ptr<view::DifficultyCatalog> m_catalog;
    std::unique_ptr<view::StatusBarView> m_statusView;
    std::unique_ptr<view::MessageBoxSolvedSink> m_solvedSink;
    std::unique_ptr<service::RecordStore> m_records;

    QAction* m_restartAction = nullptr;
    QAction* m_undoAction = nullptr;
    QLabel* m_difficultyLabel = nullptr;
    QLabel* m_movesLabel = nullptr;
    QLabel* m_timeLabel = nullptr;
    QLabel* m_bestLabel = nullptr;
};

} // namespace app
