#pragma once

/**
 * @file interfaces.h
 * @brief 控制器使用的窄输出端口。
 */

#include <QString>

#include "view/view_state.h"

namespace view
{

/// @brief 接收状态栏文本与窗口标题变化。
class StatusSink
{
  public:
    /// @brief 允许通过端口指针销毁具体实现。
    virtual ~StatusSink() = default;

    /**
     * @brief 应用一份完整的状态栏状态。
     * @param state 状态栏要显示的难度、步数、用时与最佳成绩。
     */
    virtual void applyStatus(const StatusState& state) = 0;

    /**
     * @brief 设置窗口标题。
     * @param title 新标题。
     */
    virtual void setWindowTitle(const QString& title) = 0;
};

/// @brief 接收通关结果的展示请求。
class SolvedSink
{
  public:
    /// @brief 允许通过端口指针销毁具体实现。
    virtual ~SolvedSink() = default;

    /**
     * @brief 展示通关结果。
     * @param info 难度、步数、用时与是否破纪录。
     */
    virtual void showSolved(const SolvedInfo& info) = 0;
};

} // namespace view
