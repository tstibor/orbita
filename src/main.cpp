#include <QApplication>
#include "version.h"
#include "mainwindow.h"
#include "logger.h"
#include "settings.h"
#include "sqlite.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(orbita);

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);

    QApplication app(argc, argv);

    Logger::instance()->create(Settings::FilenameLog());
    Sqlite *sqlite = new Sqlite(Settings::FilenameDb());

    bool success = sqlite->createAllTables();
    delete sqlite;
    if (!success)
        qFatal("cannot create required sqlite tables");

    MainWindow mainWin;
    mainWin.show();

    return app.exec();
}
