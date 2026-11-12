#pragma once

/**
 * @file main_window.h
 * @brief 应用的组装根与外壳窗口。
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
 * @brief 数字迷宫的组装根。
 *
 * 创建模型、被动视图、服务与控制器，然后把菜单动作接到控制器的槽上；
 * 它本身不含游戏规则，也不转发信号。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 组装模型、被动视图、服务与控制器。
     * @param parent 父窗口。
     * @note 构造末尾会完成控制器的初始同步，窗口创建后即可交互。
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /// @brief 先释放控制器，避免它晚于成员辅助对象销毁。
    ~MainWindow() override;

private:
    /// @brief 建立“游戏”与“帮助”菜单，并把动作接到控制器槽。
    void setupMenus();

    /// @brief 建立不可拖动、只含重开与悔棋两个动作的工具栏。
    void setupToolBar();

    /// @brief 建立状态栏的四个标签，并把最佳成绩放在右侧常驻区。
    void setupStatusBar();

    /// @brief 建立悔棋动作：快捷键 Ctrl+Z，初始不可用。
    void createUndoAction();

    view::BoardView* m_view = nullptr;  ///< 中央棋盘控件，由 Qt 对象树持有。
    model::GameModel* m_model = nullptr;  ///< 游戏模型，由 Qt 对象树持有。
    controller::GameController* m_controller = nullptr;  ///< 控制器；析构时先于其协作者手动释放。

    std::unique_ptr<view::DifficultyCatalog> m_catalog;  ///< 难度名称表，供菜单与控制器生成文本。
    std::unique_ptr<view::StatusBarView> m_statusView;  ///< 状态栏输出端口的实现。
    std::unique_ptr<view::MessageBoxSolvedSink> m_solvedSink;  ///< 通关结果输出端口的实现。
    std::unique_ptr<service::RecordStore> m_records;  ///< 基于 @c QSettings 的最佳成绩存储。

    QAction* m_restartAction = nullptr;  ///< 重开本局动作，同时加入菜单与工具栏。
    QAction* m_undoAction = nullptr;  ///< 悔棋动作，可用状态由状态栏同步。
    QLabel* m_difficultyLabel = nullptr;  ///< 状态栏中的难度标签。
    QLabel* m_movesLabel = nullptr;  ///< 状态栏中的步数标签。
    QLabel* m_timeLabel = nullptr;  ///< 状态栏中的用时标签。
    QLabel* m_bestLabel = nullptr;  ///< 状态栏右侧常驻的最佳成绩标签。
};

} // namespace app
