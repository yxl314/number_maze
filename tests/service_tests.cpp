#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryDir>

#include <cstdio>
#include <memory>

#include "service/record_store.h"

namespace
{

int g_failures = 0;

void check(bool ok, const char* what)
{
    std::printf("  [%s] %s\n", ok ? "通过" : "失败", what);
    if (!ok)
    {
        ++g_failures;
    }
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QTemporaryDir directory;
    if (!directory.isValid())
    {
        std::printf("无法创建临时目录\n");
        return 1;
    }
    const QString path = directory.path() + QStringLiteral("/records.ini");

    {
        service::RecordStore store(std::make_unique<QSettings>(path, QSettings::IniFormat));
        check(store.bestMoves(domain::Difficulty::Hard) == 0, "无记录时返回 0");
        store.saveBestMoves(domain::Difficulty::Hard, 42);
    }

    {
        service::RecordStore store(std::make_unique<QSettings>(path, QSettings::IniFormat));
        check(store.bestMoves(domain::Difficulty::Hard) == 42, "记录可持久化并重新读取");
        check(store.bestMoves(domain::Difficulty::Easy) == 0, "不同难度的记录互相独立");
    }

    std::printf("结论：%s（失败 %d 项）\n", g_failures == 0 ? "全部通过" : "存在失败", g_failures);
    return g_failures == 0 ? 0 : 1;
}
