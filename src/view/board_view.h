#pragma once

/**
 * @file board_view.h
 * @brief 被动棋盘视图：只负责绘制与上报用户意图。
 */

#include <QVector>
#include <QWidget>

#include "view/view_state.h"

namespace view
{

/**
 * @brief 被动棋盘视图。
 *
 * 视图既不认识模型也不认识控制器：控制器推入完整的 BoardFrame，视图只发出
 * nodeClicked。绘制所需的全部数据都由外部提供。
 */
class BoardView : public QWidget
{
    Q_OBJECT

  public:
    /**
     * @brief 构造棋盘视图，并打开鼠标跟踪。
     * @param parent 父控件。
     */
    explicit BoardView(QWidget* parent = nullptr);

    /// @brief 建议的初始尺寸。
    /// @return 560x560 的尺寸。
    QSize sizeHint() const override;

    /// @brief 允许的最小尺寸。
    /// @return 320x320 的尺寸。
    QSize minimumSizeHint() const override;

  public slots:
    /**
     * @brief 原子地应用一帧完整棋盘数据。
     *
     * 棋盘边长按取值个数的平方根向上取整。可移动标记个数与棋盘不一致时
     * 整体按全部不可移动处理；完成局面下悬停状态被清空，光标恢复为箭头。
     *
     * @param frame 棋盘取值、可移动标记与完成状态。
     */
    void applyFrame(const BoardFrame& frame);

    /**
     * @brief 设置绘制连线用的静态邻接表。
     * @param adjacency 每个节点一项的邻居下标表。
     */
    void setConnections(const QVector<QVector<int>>& adjacency);

  signals:
    /**
     * @brief 左键落在某个节点上。
     * @param index 被点中的节点下标。
     */
    void nodeClicked(int index);

  protected:
    /**
     * @brief 重绘整个棋盘。
     * @param event 绘制事件；其内容不参与绘制。
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief 左键按下时做命中检测并发出 nodeClicked。
     * @param event 鼠标事件；只处理左键，其余按键交给基类。
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief 跟踪鼠标悬停的节点并切换光标形状。
     *
     * 只有未完成且可移动的节点显示手形光标，其余位置恢复箭头。
     *
     * @param event 鼠标移动事件。
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief 鼠标离开控件时清除悬停状态。
     * @param event 离开事件；其内容不使用。
     */
    void leaveEvent(QEvent* event) override;

  private:
    /**
     * @brief 绘制棋盘底板。
     * @param painter 目标画笔。
     */
    void drawBackground(QPainter& painter) const;

    /**
     * @brief 绘制节点之间的连线。
     *
     * 每个节点只与下标更大的邻居相连，一条边因此只画一次；悬停节点的连线
     * 使用高亮颜色。
     *
     * @param painter 目标画笔。
     * @param radius 节点圆半径，决定线宽。
     */
    void drawConnections(QPainter& painter, qreal radius) const;

    /**
     * @brief 用虚线圆标出空格。
     * @param painter 目标画笔。
     * @param radius 节点圆半径。
     */
    void drawBlankMarker(QPainter& painter, qreal radius) const;

    /**
     * @brief 绘制所有非空节点上的数字。
     *
     * 数字过宽时按比例缩小字号，保证落在圆内。
     *
     * @param painter 目标画笔。
     * @param radius 节点圆半径。
     */
    void drawTiles(QPainter& painter, qreal radius) const;

    /**
     * @brief 在完成局面上叠加半透明遮罩与步数文字。
     * @param painter 目标画笔。
     */
    void drawSolvedOverlay(QPainter& painter) const;

    /// @brief 棋盘方形绘制区域。
    /// @return 居中、四周各留 4.5% 边距的方形。
    QRectF boardRect() const;

    /**
     * @brief 节点圆心坐标。
     * @param index 节点下标。
     * @return 所在单元格中心的坐标。
     */
    QPointF centerOf(int index) const;

    /// @brief 节点圆半径。
    /// @return 单元格边长的 0.38 倍。
    qreal circleRadius() const;

    /**
     * @brief 命中的节点。
     * @param position 控件坐标。
     * @return 落在圆内的节点下标；没有命中时为 -1。
     */
    int nodeAt(const QPoint& position) const;

    /// @brief 节点个数。
    /// @return 当前棋盘取值个数。
    int nodeCount() const
    {
        return m_board.size();
    }

    QVector<int> m_board;              ///< 当前帧的节点取值；0 表示空格。
    QVector<QVector<int>> m_adjacency; ///< 绘制连线用的邻居表。
    QVector<bool> m_movable;           ///< 每个节点的可移动标记，长度与取值个数一致。
    bool m_solved = false;             ///< 当前帧是否已完成。
    int m_solvedMoves = 0;             ///< 完成遮罩中显示的步数。
    int m_boardSize = 4;               ///< 棋盘边长，由取值个数的平方根向上取整。
    int m_hover = -1;                  ///< 鼠标悬停的节点下标；没有悬停时为 -1。
};

} // namespace view
