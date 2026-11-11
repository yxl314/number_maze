#include "view/dialogs.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QTime>

namespace view
{

void showAbout(QWidget* parent)
{
    QMessageBox::about(parent,
                       QCoreApplication::translate("Dialogs", "关于 数字迷宫"),
                       QCoreApplication::translate(
                           "Dialogs",
                           "<h3>数字迷宫</h3>"
                           "<p>把 1 到 15 的数字重新排回顺序，空格归位右下角即可通关。</p>"
                           "<p>本作的连接并非规则网格，而是包含对角线在内的特殊拓扑，"
                           "因此不能凭空间直觉判断可移动性，难度高于普通数字华容道。</p>"
                           "<p>点击与空格相连的数字即可将其滑入空格。"
                           "绿色高亮表示当前可移动，连线展示了完整的连接关系。</p>"));
}

MessageBoxSolvedSink::MessageBoxSolvedSink(QWidget* parent)
    : m_parent(parent)
{
}

void MessageBoxSolvedSink::showSolved(const SolvedInfo& info)
{
    QString text = QCoreApplication::translate(
                       "Dialogs",
                       "恭喜完成！\n\n难度：%1\n用步数：%2\n耗时：%3")
                       .arg(info.difficulty)
                       .arg(info.moves)
                       .arg(QTime(0, 0).addSecs(info.seconds).toString(QStringLiteral("hh:mm:ss")));
    if (info.newRecord)
    {
        text += QCoreApplication::translate("Dialogs", "\n\n这是该难度的新纪录！");
    }

    QMessageBox box(QMessageBox::Information,
                    QCoreApplication::translate("Dialogs", "数字迷宫"),
                    text,
                    QMessageBox::Ok,
                    m_parent);
    box.exec();
}

} // namespace view
