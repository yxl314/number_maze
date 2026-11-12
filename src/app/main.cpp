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

#include <singleproc/singleproc.hpp>
#include <stacklog/stacklog.h>

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
    stacklog::Config config;
    config.dir = stateLogDirectory().toStdString();
    config.base_name = g_application_name;
    config.max_file_size = 2ULL * 1024 * 1024;
    config.max_files = 3;
    config.level = stacklog::Level::Info;
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
            exitCode = QApplication::exec();
            STACKLOG_INFO() << "数字迷宫退出 code=" << exitCode;
        }
    }

    stacklog::shutdown();
    return exitCode;
}
