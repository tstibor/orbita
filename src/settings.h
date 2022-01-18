#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCoreApplication>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include "version.h"

class Settings
{

public:
    static Settings* instance();

    const static QString FilenameDb();
    const static QString FilenameLog();
    const static QString Path();

private:
    explicit Settings();
    QString m_appDataLocationPath;
};

#endif // SETTINGS_H
