#include "settings.h"

Settings *Settings::instance()
{
    static Settings settings;

    return &settings;
}

Settings::Settings()
{
    if (QCoreApplication::applicationName().isEmpty())
	QCoreApplication::setApplicationName("orbita");

    if (QCoreApplication::applicationVersion().isEmpty())
	QCoreApplication::setApplicationVersion(orbita_VERSION);

    m_appDataLocationPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (m_appDataLocationPath.isEmpty())
	qFatal("AppDataLocation path name does not exist");

    qDebug() << "AppDataLocation path" << m_appDataLocationPath;

    QDir dir(m_appDataLocationPath);
    if (!dir.exists()) {
	qWarning("AppDataLocation directory %s does not exist", qUtf8Printable(m_appDataLocationPath));
	if (dir.mkdir(m_appDataLocationPath))
	    qInfo("AppDataLocation directory %s created", qUtf8Printable(m_appDataLocationPath));
	else
	    qFatal("AppDataLocation directory %s cannot be created", qUtf8Printable(m_appDataLocationPath));
    }
}

const QString Settings::FilenameDb()
{
    return instance()->m_appDataLocationPath + "/" +
	QCoreApplication::applicationName() + ".db";
}
const QString Settings::FilenameLog()
{
    return instance()->m_appDataLocationPath + "/" +
	QCoreApplication::applicationName() + ".log";
}

const QString Settings::Path()
{
    return instance()->m_appDataLocationPath;
}
