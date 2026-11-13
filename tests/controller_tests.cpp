#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

#include <memory>

#include "controller/game_controller.h"
#include "model/game_model.h"
#include "service/record_store.h"
#include "view/board_view.h"
#include "view/difficulty_catalog.h"
#include "view/interfaces.h"

namespace
{

class FakeStatus : public view::StatusSink
{
  public:
    void applyStatus(const view::StatusState& state) override
    {
        last = state;
        ++count;
    }

    void setWindowTitle(const QString& value) override
    {
        title = value;
    }

    view::StatusState last;
    QString title;
    int count = 0;
};

class FakeSolved : public view::SolvedSink
{
  public:
    void showSolved(const view::SolvedInfo& info) override
    {
        last = info;
        ++count;
    }

    view::SolvedInfo last;
    int count = 0;
};

int firstMovable(const QVector<bool>& movable)
{
    for (int i = 0; i < movable.size(); ++i)
    {
        if (movable.at(i))
        {
            return i;
        }
    }
    return -1;
}

} // namespace

class ControllerTests : public QObject
{
    Q_OBJECT

  private slots:
    void wiringAndStateFlow();
};

void ControllerTests::wiringAndStateFlow()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.path() + QStringLiteral("/records.ini");

    service::RecordStore records(std::make_unique<QSettings>(path, QSettings::IniFormat));
    records.saveBestMoves(domain::Difficulty::Hard, 10);

    model::GameModel model;
    view::BoardView view;
    view::DifficultyCatalog catalog;
    FakeStatus status;
    FakeSolved solved;
    controller::GameController controller(&view, &model, &status, &solved, &records, &catalog);

    controller.initialize();

    QCOMPARE(status.last.difficulty, QStringLiteral("普通"));
    QCOMPARE(status.title, QStringLiteral("数字迷宫 - 普通"));
    QCOMPARE(status.last.moves, 0);
    QVERIFY(status.last.canUndo == false);

    const int movable = firstMovable(model.movableData());
    QVERIFY(movable >= 0);
    emit view.nodeClicked(movable);

    QCOMPARE(model.moveCount(), 1);
    QCOMPARE(status.last.moves, 1);
    QCOMPARE(status.last.canUndo, true);

    controller.handleUndo();
    QCOMPARE(model.moveCount(), 0);
    QCOMPARE(status.last.moves, 0);

    const QVector<int> initial = model.boardData();
    const int nextMovable = firstMovable(model.movableData());
    QVERIFY(nextMovable >= 0);
    emit view.nodeClicked(nextMovable);
    QVERIFY(model.boardData() != initial);
    controller.handleRestart();
    QCOMPARE(model.boardData(), initial);
    QCOMPARE(model.moveCount(), 0);

    controller.handleNewGame(domain::Difficulty::Hard);
    QCOMPARE(model.difficulty(), domain::Difficulty::Hard);
    QCOMPARE(status.last.difficulty, QStringLiteral("困难"));
    QCOMPARE(model.bestRecord(domain::Difficulty::Hard), 10);
    QCOMPARE(status.last.bestMoves, 10);
    QCOMPARE(solved.count, 0);
}

QTEST_MAIN(ControllerTests)

#include "controller_tests.moc"
