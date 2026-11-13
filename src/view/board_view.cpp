#include "view/board_view.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>

namespace view
{

namespace
{

const QColor g_board_color(40, 44, 52);
const QColor g_edge_color(96, 104, 118);
const QColor g_edge_hot_color(152, 162, 180);
const QColor g_tile_color(70, 130, 220);
const QColor g_tile_hover_color(120, 176, 255);
const QColor g_tile_movable_color(58, 176, 122);
const QColor g_tile_text_color(255, 255, 255);
const QColor g_blank_color(130, 136, 148);
const QColor g_solved_color(255, 214, 102);

} // namespace

BoardView::BoardView(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(QSize(320, 320));
}

QSize BoardView::sizeHint() const
{
    return QSize(560, 560);
}

QSize BoardView::minimumSizeHint() const
{
    return QSize(320, 320);
}

void BoardView::applyFrame(const BoardFrame& frame)
{
    m_board = frame.board;
    const int total = m_board.size();
    int side = 1;
    while (side * side < total)
    {
        ++side;
    }
    m_boardSize = side;

    m_movable = frame.movable;
    if (m_movable.size() != total)
    {
        m_movable = QVector<bool>(total, false);
    }
    m_solved = frame.solved;
    m_solvedMoves = frame.solvedMoves;
    if (m_solved)
    {
        m_hover = -1;
        setCursor(Qt::ArrowCursor);
    }
    update();
}

void BoardView::setConnections(const QVector<QVector<int>>& adjacency)
{
    m_adjacency = adjacency;
    update();
}

QRectF BoardView::boardRect() const
{
    const qreal side = qMin(width(), height());
    const qreal margin = side * 0.045;
    return QRectF(((width() - side) / 2.0) + margin, ((height() - side) / 2.0) + margin, side - (2 * margin),
                  side - (2 * margin));
}

QPointF BoardView::centerOf(int index) const
{
    const QRectF board = boardRect();
    const qreal cell = qMin(board.width(), board.height()) / m_boardSize;
    const int row = index / m_boardSize;
    const int col = index % m_boardSize;
    return QPointF(board.left() + ((col + 0.5) * cell), board.top() + ((row + 0.5) * cell));
}

qreal BoardView::circleRadius() const
{
    const QRectF board = boardRect();
    return qMin(board.width(), board.height()) / m_boardSize * 0.38;
}

int BoardView::nodeAt(const QPoint& position) const
{
    const qreal radius = circleRadius();
    for (int i = 0; i < nodeCount(); ++i)
    {
        const QPointF delta = centerOf(i) - QPointF(position);
        if ((delta.x() * delta.x()) + (delta.y() * delta.y()) <= radius * radius)
        {
            return i;
        }
    }
    return -1;
}

void BoardView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    const qreal radius = circleRadius();
    drawBackground(painter);
    drawConnections(painter, radius);
    drawBlankMarker(painter, radius);
    drawTiles(painter, radius);
    if (m_solved)
    {
        drawSolvedOverlay(painter);
    }
}

void BoardView::drawBackground(QPainter& painter) const
{
    const QRectF board = boardRect();
    const qreal corner = board.width() * 0.032;
    painter.setPen(Qt::NoPen);
    painter.setBrush(g_board_color);
    painter.drawRoundedRect(QRectF(rect()).adjusted(2, 2, -2, -2), corner, corner);
}

void BoardView::drawConnections(QPainter& painter, qreal radius) const
{
    const qreal width = qMax(1.5, radius * 0.115);
    for (int i = 0; i < m_adjacency.size(); ++i)
    {
        for (const int j : m_adjacency.at(i))
        {
            if (j <= i || j >= nodeCount())
            {
                continue;
            }
            const bool hot = (i == m_hover || j == m_hover);
            painter.setPen(QPen(hot ? g_edge_hot_color : g_edge_color, width, Qt::SolidLine, Qt::RoundCap));
            painter.drawLine(centerOf(i), centerOf(j));
        }
    }
}

void BoardView::drawBlankMarker(QPainter& painter, qreal radius) const
{
    int blank = -1;
    for (int i = 0; i < nodeCount(); ++i)
    {
        if (m_board.at(i) == 0)
        {
            blank = i;
            break;
        }
    }
    if (blank < 0)
    {
        return;
    }

    painter.setPen(QPen(g_blank_color, qMax(1.5, radius * 0.10), Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(centerOf(blank), radius, radius);
}

void BoardView::drawTiles(QPainter& painter, qreal radius) const
{
    QFont font = this->font();
    font.setBold(true);
    font.setPixelSize(qMax(10, int(radius * 1.05)));
    painter.setFont(font);

    for (int i = 0; i < nodeCount(); ++i)
    {
        const int value = m_board.at(i);
        if (value == 0)
        {
            continue;
        }

        const QPointF center = centerOf(i);
        const bool movable = i < m_movable.size() && m_movable.at(i);
        QColor fill = g_tile_color;
        if (i == m_hover && movable)
        {
            fill = g_tile_hover_color;
        }
        else if (movable)
        {
            fill = g_tile_movable_color;
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 60));
        painter.drawEllipse(center + QPointF(0, radius * 0.06), radius, radius);

        painter.setBrush(fill);
        painter.setPen(QPen(fill.darker(150), qMax(1.0, radius * 0.09)));
        painter.drawEllipse(center, radius, radius);

        const QString text = QString::number(value);
        QFontMetrics metrics(font);
        const qreal maxWidth = radius * 1.44;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        const int textWidth = metrics.horizontalAdvance(text);
#else
        const int textWidth = metrics.width(text);
#endif
        if (textWidth > maxWidth)
        {
            QFont small = font;
            small.setPixelSize(qMax(8, int(font.pixelSize() * maxWidth / textWidth)));
            painter.setFont(small);
        }

        painter.setPen(g_tile_text_color);
        painter.drawText(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius), Qt::AlignCenter,
                         text);
        painter.setFont(font);
    }
}

void BoardView::drawSolvedOverlay(QPainter& painter) const
{
    const QRectF board = boardRect();
    const qreal corner = board.width() * 0.03;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 130));
    painter.drawRoundedRect(board, corner, corner);

    QFont font = this->font();
    font.setBold(true);
    font.setPixelSize(qMax(16, int(board.width() * 0.085)));
    painter.setFont(font);
    painter.setPen(g_solved_color);
    painter.drawText(board, Qt::AlignCenter, QStringLiteral("完成！\n共 %1 步").arg(m_solvedMoves));
}

void BoardView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    const int index = nodeAt(event->pos());
    if (index >= 0)
    {
        emit nodeClicked(index);
    }
}

void BoardView::mouseMoveEvent(QMouseEvent* event)
{
    const int index = nodeAt(event->pos());
    if (index == m_hover)
    {
        return;
    }

    m_hover = index;
    const bool clickable = index >= 0 && index < m_movable.size() && m_movable.at(index) && !m_solved;
    setCursor(clickable ? Qt::PointingHandCursor : Qt::ArrowCursor);
    update();
}

void BoardView::leaveEvent(QEvent* /*event*/)
{
    if (m_hover == -1)
    {
        return;
    }
    m_hover = -1;
    setCursor(Qt::ArrowCursor);
    update();
}

} // namespace view
