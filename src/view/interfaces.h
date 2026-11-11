#pragma once

/**
 * @file interfaces.h
 * @brief Narrow output ports used by the controller.
 */

#include <QString>

#include "view/view_state.h"

namespace view
{

/// @brief Receives status-bar text and window-title changes.
class StatusSink
{
public:
    virtual ~StatusSink() = default;

    /// @param state Complete status-bar state.
    virtual void applyStatus(const StatusState& state) = 0;

    /// @param title Window title.
    virtual void setWindowTitle(const QString& title) = 0;
};

/// @brief Receives the solved result presentation.
class SolvedSink
{
public:
    virtual ~SolvedSink() = default;

    /// @param info Solved result.
    virtual void showSolved(const SolvedInfo& info) = 0;
};

} // namespace view
