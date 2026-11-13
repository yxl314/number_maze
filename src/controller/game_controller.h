#pragma once

/**
 * @file game_controller.h
 * @brief 数字迷宫的被动视图 MVC 控制器。
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
 * @brief 协调被动棋盘视图、模型与外壳输出端口的控制器。
 *
 * 控制器是唯一同时持有可写模型和具体视图指针的对象；它不含游戏规则，
 * 也不含绘制代码。
 */
class GameController : public QObject
{
    Q_OBJECT

  public:
    /**
     * @brief 保存各协作者，此时不做信号连接。
     *
     * 各指针只保存、不接管所有权，因此都必须比控制器活得更久。
     *
     * @param view 被动棋盘视图。
     * @param model 游戏模型。
     * @param status 状态栏输出端口。
     * @param solved 通关结果输出端口。
     * @param records 最佳成绩存储。
     * @param catalog 难度名称表，用于生成界面文本。
     * @param parent Qt 对象树的父对象。
     */
    GameController(view::BoardView* view, model::GameModel* model, view::StatusSink* status, view::SolvedSink* solved,
                   service::RecordStore* records, const view::DifficultyCatalog* catalog, QObject* parent = nullptr);

    /// @brief 连接信号、载入纪录并做一次初始同步。
    void initialize();

  public slots:
    /**
     * @brief 开始指定难度的新一局。
     * @param level 菜单中选定的难度。
     */
    void handleNewGame(domain::Difficulty level);

    /// @brief 重开本局。
    void handleRestart();

    /// @brief 撤销最近一次移动。
    void handleUndo();

  private slots:
    /**
     * @brief 把点击转成模型的移动请求。
     * @param index 被点击的节点下标。
     */
    void handleNodeClicked(int index);

    /// @brief 模型棋盘变化后刷新视图。
    void handleBoardChanged();

    /// @brief 模型状态变化后刷新状态栏与标题。
    void handleStatusChanged();

    /**
     * @brief 处理通关：按需写入新纪录、刷新状态并弹出结果。
     * @param moves 本局步数。
     * @param seconds 本局用时（秒）。
     * @param newRecord 是否刷新了该难度的最佳步数。
     */
    void handleSolved(int moves, int seconds, bool newRecord);

  private:
    /// @brief 用模型当前状态组装一帧完整的 BoardFrame 推给视图。
    void syncBoardToView();

    /// @brief 用模型当前状态刷新状态栏文本与窗口标题。
    void refreshStatus();

    /// @brief 从持久化存储读出各难度最佳步数并写入模型。
    void loadRecords();

    view::BoardView* m_view = nullptr;                  ///< 被动棋盘视图，不持有所有权。
    model::GameModel* m_model = nullptr;                ///< 游戏模型，不持有所有权。
    view::StatusSink* m_status = nullptr;               ///< 状态栏输出端口，可为空。
    view::SolvedSink* m_solved = nullptr;               ///< 通关结果输出端口，可为空。
    service::RecordStore* m_records = nullptr;          ///< 最佳成绩存储，可为空。
    const view::DifficultyCatalog* m_catalog = nullptr; ///< 难度名称表，可为空。
};

} // namespace controller
