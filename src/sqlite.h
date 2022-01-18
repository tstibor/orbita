#ifndef SQLITE_H
#define SQLITE_H

#include <QtSql>
#include "settings.h"

#define QUERY           "SELECT * FROM "
#define QUERY_NUM_ROWS	"SELECT COUNT(*) FROM "

#define TABLE_SETTING           "settings"

#define TABLE_ASTEROID          "asteroids"
#define QUERY_ASTEROID		QUERY          TABLE_ASTEROID
#define QUERY_ASTEROID_NUM_ROWS	QUERY_NUM_ROWS TABLE_ASTEROID

#define TABLE_COMET	        "comets"
#define QUERY_COMET		QUERY          TABLE_COMET
#define QUERY_COMET_NUM_ROWS	QUERY_NUM_ROWS TABLE_COMET

enum class SqlTable { SETTING, ASTEROID, COMET };

class Sqlite : public QObject
{
    Q_OBJECT

public:
    Sqlite(const QString &filename, const QString &connName = "orbita");
    ~Sqlite();
    QSqlDatabase getDb();
    bool createTable(SqlTable sqlTable);
    bool createAllTables();

    static bool writeSettings(const QMap<QString, QVariant> &map,
                              const QString &filenameDb = Settings::FilenameDb());
    static bool writeSetting(const QString &key, const QVariant &value,
                             const QString &filenameDb = Settings::FilenameDb());
    static bool readSettings(QMap<QString, QVariant> &map,
                             const QString &filenameDb = Settings::FilenameDb());
    static QVariant readSetting(const QString &key,
                                const QString &filenameDb = Settings::FilenameDb());

private:
    QString m_connectionName;

public slots:
    bool addEntryAsteroid(const QVariantList &orbEntry);
    bool addEntryComet(const QVariantList &orbEntry);
};

#endif // SQLITE_H
