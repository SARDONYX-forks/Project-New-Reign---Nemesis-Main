#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QResource>
#include <Qdir>

static std::string qToStd(const QString& s)
{
    return s.toUtf8().toStdString();
}

static void dumpQrcImpl(const QString& path, std::string& out)
{
    QDir dir(path);

    out += "[QRC] DIR: " + qToStd(path) + "\n";

    for (const QString& entry : dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
    {
        QString full = path;
        if (!full.endsWith('/')) full += '/';
        full += entry;

        out += "  - " + qToStd(full) + "\n";

        if (QDir(full).exists()) dumpQrcImpl(full, out);
    }
}

std::string makeQrcDump()
{
    std::string dump;
    dumpQrcImpl(":/", dump);
    return dump;
}
