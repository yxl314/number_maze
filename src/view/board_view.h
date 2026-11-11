#pragma once

/**
 * @file board_view.h
 * @brief Passive board view: painting and user-intent signals only.
 */

#include <QVector>
#include <QWidget>

#include "view/view_state.h"

namespace view
{

/**
 * @brief Passive board view.
 *
 * The view knows neither the model nor the controller: the controller pushes
 * complete BoardFrame values, and the view only emits nodeClicked. All drawing
 * data is supplied externally.
 */
class BoardView : public QWidget
{
    Q_OBJECT

public:
    explicit BoardView(QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    /// @brief Apply one complete board frame atomically.
    void applyFrame(const BoardFrame& frame);

    /// @brief Set the static adjacency table used for drawing connections.
    void setConnections(const QVector<QVector<int>>& adjacency);

signals:
    /// @param index Clicked node index.
    void nodeClicked(int index);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void drawBackground(QPainter& painter) const;
    void drawConnections(QPainter& painter, qreal radius) const;
    void drawBlankMarker(QPainter& painter, qreal radius) const;
    void drawTiles(QPainter& painter, qreal radius) const;
    void drawSolvedOverlay(QPainter& painter) const;

    QRectF boardRect() const;
    QPointF centerOf(int index) const;
    qreal circleRadius() const;
    int nodeAt(const QPoint& position) const;
    int nodeCount() const
    {
        return m_board.size();
    }

    QVector<int> m_board;
    QVector<QVector<int>> m_adjacency;
    QVector<bool> m_movable;
    bool m_solved = false;
    int m_solvedMoves = 0;
    int m_boardSize = 4;
    int m_hover = -1;
};

} // namespace view
