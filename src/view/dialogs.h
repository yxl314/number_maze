#pragma once

/**
 * @file dialogs.h
 * @brief 本地化的消息框展示辅助。
 */

#include "view/interfaces.h"

class QWidget;

namespace view
{

/**
 * @brief 显示“关于”对话框。
 * @param parent 对话框父控件。
 */
void showAbout(QWidget* parent);

/// @brief 基于 QMessageBox 的 SolvedSink 实现。
class MessageBoxSolvedSink : public SolvedSink
{
  public:
    /**
     * @brief 保存消息框的父控件。
     * @param parent 消息框父控件。
     */
    explicit MessageBoxSolvedSink(QWidget* parent = nullptr);

    /**
     * @brief 弹出通关结果。
     * @param info 难度、步数、用时与是否破纪录。
     */
    void showSolved(const SolvedInfo& info) override;

  private:
    QWidget* m_parent = nullptr; ///< 消息框的父控件，可为空。
};

} // namespace view
