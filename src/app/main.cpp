/**
 * @file main.cpp
 * @brief 程序入口：装配日志、单实例守卫与主窗口。
 */

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>

#include <singleproc/singleproc.hpp>
#include <stacklog/stacklog.hpp>

#include "app/main_window.h"

namespace
{

/// 单实例锁 key。
constexpr const char* g_instance_name = "number-maze";

/// 日志文件名。
constexpr const char* g_application_name = "number-maze";

/**
 * @brief 加载随程序内置的中文字体，避免目标系统缺少 CJK 字体时出现方框。
 *
 * 字体是 Noto Sans CJK SC 的子集，只包含本程序界面用到的字符。
 * 加载失败不致命，退回系统默认字体。
 */
void setupUiFont()
{
    const int fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/NotoSansCJKsc-Regular.otf"));
    if (fontId < 0)
    {
        return;
    }

    const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (families.isEmpty())
    {
        return;
    }

    QFont font = QApplication::font();
    font.setFamily(families.at(0));
    QApplication::setFont(font);
}

/**
 * @brief 计算日志目录：$XDG_STATE_HOME/number-maze。
 *
 * 未设置 XDG_STATE_HOME 时使用 ~/.local/state。
 */
QString stateLogDirectory()
{
    QString stateHome = QString::fromLocal8Bit(qgetenv("XDG_STATE_HOME"));
    if (stateHome.isEmpty())
    {
        stateHome = QDir::homePath() + QStringLiteral("/.local/state");
    }
    return stateHome + QStringLiteral("/number-maze");
}

/**
 * @brief 初始化 stacklog；失败时库会自动降级到 stderr。
 */
bool setupLogging()
{
    stacklog::config config;
    config.dir = stateLogDirectory().toStdString();
    config.base_name = g_application_name;
    config.max_file_size = 2ULL * 1024 * 1024;
    config.max_files = 3;
    config.min_level = stacklog::level::info;
    return stacklog::init(config);
}

} // namespace

/**
 * @brief 程序入口。
 *
 * @par 流程
 *  1. 初始化日志；
 *  2. 取单实例锁：已有实例或锁错误时直接结束，不再打开窗口；
 *  3. 取得独占权后创建主窗口并进入事件循环。
 *
 * @note 当前应用暂未接入 singleproc 门铃；重复启动会直接退出，不会把已有
 *       窗口带到前台。
 */
// 审计用：GUI 程序没有命令行退出路径，无头环境里只能被杀掉——那样拿不到
// 正常退出流程产生的泄漏汇总。用定时器让窗口自己关，Qt 走完整的 close →
// quit → 析构链路。只在显式设置了 CXX_GUI_AUTOCLOSE_MS 时生效，正常启动
// 不受任何影响。
static void installAutoClose(QWidget* window)
{
    const QByteArray raw = qgetenv("CXX_GUI_AUTOCLOSE_MS");
    bool ok = false;
    const int milliseconds = raw.toInt(&ok);
    if (!ok || milliseconds <= 0)
    {
        return;
    }
    QTimer::singleShot(milliseconds, window, [window]() { window->close(); });
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle(QStringLiteral("Fusion"));
    setupUiFont();
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/number-maze.png")));

    QApplication::setApplicationName(QStringLiteral("数字迷宫"));
    QApplication::setOrganizationName(QStringLiteral("NumberMaze"));
    QApplication::setApplicationVersion(QStringLiteral("2.0.0"));

    const bool logReady = setupLogging();

    int exitCode = 0;
    {
        STACKLOG_TRACE("main");

        if (!logReady)
        {
            STACKLOG_WARN() << "日志未能落盘，仅在标准错误输出";
        }
        STACKLOG_INFO() << "数字迷宫启动";

        singleproc::Guard guard(g_instance_name);
        if (guard.state() == singleproc::Guard::State::Error)
        {
            STACKLOG_ERROR() << "单实例锁失败 errno=" << guard.error_code();
            exitCode = 1;
        }
        else if (!guard.is_primary())
        {
            STACKLOG_INFO() << "已有实例在运行，本次启动不再打开窗口";
            exitCode = 0;
        }
        else
        {
            app::MainWindow window;
            window.show();
            installAutoClose(&window);
            exitCode = QApplication::exec();
            STACKLOG_INFO() << "数字迷宫退出 code=" << exitCode;
        }
    }

    stacklog::shutdown();
    return exitCode;
}
