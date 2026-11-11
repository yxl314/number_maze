#pragma once

/**
 * @file dialogs.h
 * @brief Localized message-box presentation helpers.
 */

#include "view/interfaces.h"

class QWidget;

namespace view
{

/// @brief Show the application about box.
/// @param parent Dialog parent.
void showAbout(QWidget* parent);

/// @brief QMessageBox-backed implementation of SolvedSink.
class MessageBoxSolvedSink : public SolvedSink
{
public:
    /// @param parent Dialog parent.
    explicit MessageBoxSolvedSink(QWidget* parent = nullptr);

    void showSolved(const SolvedInfo& info) override;

private:
    QWidget* m_parent = nullptr;
};

} // namespace view
