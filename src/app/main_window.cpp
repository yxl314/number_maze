#include "app/main_window.h"

#include <QtWidgets/QAction>
#include <QApplication>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

#include <utility>

#include "controller/game_controller.h"
#include "model/game_model.h"
#include "service/record_store.h"
#include "view/board_view.h"
#include "view/dialogs.h"
#include "view/difficulty_catalog.h"
#include "view/status_bar_view.h"

namespace app
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("数字迷宫"));

    m_view = new view::BoardView(this);
    m_model = new model::GameModel(this);
    setCentralWidget(m_view);

    setupStatusBar();
    createUndoAction();

    m_catalog = std::make_unique<view::DifficultyCatalog>();
    m_statusView = std::make_unique<view::StatusBarView>(
        this, m_difficultyLabel, m_movesLabel, m_timeLabel, m_bestLabel, m_undoAction);
    m_solvedSink = std::make_unique<view::MessageBoxSolvedSink>(this);
    m_records = std::make_unique<service::RecordStore>(
        std::make_unique<QSettings>());

    m_controller = new controller::GameController(m_view,
                                                  m_model,
                                                  m_statusView.get(),
                                                  m_solvedSink.get(),
                                                  m_records.get(),
                                                  m_catalog.get(),
                                                  this);
    connect(m_undoAction, &QAction::triggered, m_controller, &controller::GameController::handleUndo);

    setupMenus();
    setupToolBar();
    m_controller->initialize();

    resize(620, 760);
}

MainWindow::~MainWindow()
{
    // Status/solved/record helpers are members and die after this body; the
    // controller must not outlive them.
    delete m_controller;
    m_controller = nullptr;
}

void MainWindow::createUndoAction()
{
    m_undoAction = new QAction(QStringLiteral("悔棋(&U)"), this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setEnabled(false);
}

void MainWindow::setupMenus()
{
    QMenu* gameMenu = menuBar()->addMenu(QStringLiteral("游戏(&G)"));
    QMenu* newMenu = gameMenu->addMenu(QStringLiteral("新游戏(&N)"));
    for (const domain::Difficulty level : m_catalog->all())
    {
        QAction* action = newMenu->addAction(m_catalog->name(level));
        connect(action, &QAction::triggered, this, [this, level] { m_controller->handleNewGame(level); });
    }

    gameMenu->addSeparator();
    m_restartAction = gameMenu->addAction(QStringLiteral("重开本局(&R)"));
    m_restartAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));
    connect(m_restartAction, &QAction::triggered, m_controller, &controller::GameController::handleRestart);

    gameMenu->addAction(m_undoAction);
    gameMenu->addSeparator();

    QAction* quitAction = gameMenu->addAction(QStringLiteral("退出(&Q)"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    QMenu* helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
    QAction* aboutAction = helpMenu->addAction(QStringLiteral("关于(&A)"));
    connect(aboutAction, &QAction::triggered, this, [this] { view::showAbout(this); });
}

void MainWindow::setupToolBar()
{
    QToolBar* bar = addToolBar(QStringLiteral("操作"));
    bar->setMovable(false);
    bar->addAction(m_restartAction);
    bar->addAction(m_undoAction);
}

void MainWindow::setupStatusBar()
{
    m_difficultyLabel = new QLabel(this);
    m_movesLabel = new QLabel(this);
    m_timeLabel = new QLabel(this);
    m_bestLabel = new QLabel(this);

    statusBar()->addWidget(m_difficultyLabel);
    statusBar()->addWidget(m_movesLabel);
    statusBar()->addWidget(m_timeLabel);
    statusBar()->addPermanentWidget(m_bestLabel);
}

} // namespace app
