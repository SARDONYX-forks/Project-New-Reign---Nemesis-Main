#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QResource>

#include "AppConfig.h"
#include "AppLauncher.h"
#include "DebugDump.h"
#include "Logger.h"
#include "ModModelHandler.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/icon.png"));

    try
    {
        Logger::SetPath(std::filesystem::current_path() / "Nemesis_App.log");
        Logger::ClearLog();

        QQmlApplicationEngine engine;

        AppConfig app_config(std::filesystem::path(argv[0]).parent_path() / L"nemesis.ini");
        AppLauncher app_launcher;
        ModModelHandler mod_handler(app_config);

        QQmlContext* root_context = engine.rootContext();

        root_context->setContextProperty("appConfig", &app_config);
        root_context->setContextProperty("appLauncher", &app_launcher);
        root_context->setContextProperty("modHandler", &mod_handler);
        root_context->setContextProperty("appDirectoryPath", QGuiApplication::applicationDirPath());
        root_context->setContextProperty("appExecutablePath", QGuiApplication::applicationFilePath());

        QObject::connect(&engine,
                         &QQmlApplicationEngine::objectCreationFailed,
                         &engine,
                         []() { qCritical() << "QML object creation failed"; });
        QObject::connect(&engine,
                         &QQmlApplicationEngine::warnings,
                         [](const QList<QQmlError>& warnings)
                         {
                             for (const auto& w : warnings)
                                 Logger::Log(w.toString().toStdString());
                         });
        QObject::connect(&app, &QApplication::aboutToQuit, &app_launcher, &AppLauncher::quitRunningProcess);

        engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
        if (engine.rootObjects().isEmpty())
        {
            Logger::Log(makeQrcDump());
            throw std::runtime_error("No root QML objects loaded");
        }
        return app.exec();
    }
    catch (const std::exception& ex)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(QObject::tr("Exception Caught!"));
        msgBox.setInformativeText(ex.what());
        msgBox.setStandardButtons(QMessageBox::Abort);
        msgBox.setWindowTitle(QObject::tr("Application Error"));
        msgBox.exec();
        return -1;
    }
}
