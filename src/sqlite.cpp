#include "sqlite.h"

Sqlite::Sqlite(const QString &filename, const QString &connName)
{
    Qt::HANDLE threadId = QThread::currentThreadId();
    m_connectionName =
            QString(connName) + QString("-%1").arg(reinterpret_cast<qlonglong>(threadId));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(filename);
}

Sqlite::~Sqlite()
{
    getDb().close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

QSqlDatabase Sqlite::getDb()
{
    return QSqlDatabase::database(m_connectionName);
}

bool Sqlite::createAllTables()
{
    return (createTable(SqlTable::ASTEROID) & createTable(SqlTable::COMET)
            & createTable(SqlTable::SETTING));
}

bool Sqlite::createTable(SqlTable sqlTable)
{
    QSqlDatabase db = getDb();

    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    QString sqlQuery;

    switch (sqlTable) {
    /* Orbital elements asteroid: e, a, i, Ω, ω, M_0. */
    case SqlTable::ASTEROID: {
        sqlQuery =
	    "CREATE TABLE IF NOT EXISTS " TABLE_ASTEROID
	    "(name varchar(29) PRIMARY KEY,"
	    "eccentricity real," /* e */
	    "semimajor real,"	 /* a */
	    "inclination real,"	 /* i */
	    "ascending real,"	 /* Ω */
	    "perihelion real,"	 /* ω */
	    "anomaly real,"	 /* M_0 */
	    "motion real,"
	    "epoch varchar(10),"
	    "type integer,"
	    "designation varchar(7),"
	    "observations integer,"
	    "last varchar(9),"
	    "H real,"
	    "G real);";
        break;
    }
    /* Orbital elements comet: e, q, i, Ω, ω, T_0. */
    case SqlTable::COMET: {
        sqlQuery = "CREATE TABLE IF NOT EXISTS " TABLE_COMET
	    "(name varchar(59) PRIMARY KEY,"
	    "eccentricity real,"
	    "inclination real,"
	    "ascending real,"
	    "perihelion real,"
	    "perihelion_year integer,"
	    "perihelion_month integer,"
	    "perihelion_day real,"
	    "perihelion_distance real,"
            "type varchar(16),"
	    "H real,"
	    "G real);";
        break;
    }
    case SqlTable::SETTING: {
        sqlQuery = "CREATE TABLE IF NOT EXISTS " TABLE_SETTING " (key TEXT PRIMARY KEY, value TEXT);";
        break;
    }
    default:
        return false;
    }

    if (!query.exec(sqlQuery)) {
        qWarning() << "sql query" << sqlQuery << "failed" << query.lastError().databaseText();
        return false;
    }

    return true;
}

bool Sqlite::addEntryAsteroid(const struct asteroid_t &asteroid)
{
    QSqlDatabase db = getDb();

    if (!db.isOpen())
        return false;

    /* Significant speed up, using PRAGMA and TRANSCATION. */
    db.exec("PRAGMA synchronous = OFF");
    db.exec("PRAGMA Journal_mode = MEMORY");
    db.exec("BEGIN TRANSCATION");

    QSqlQuery query(db);
    const QString prepare =
	"REPLACE INTO " TABLE_ASTEROID "("
	"name,"
	"eccentricity,"
	"semimajor,"
	"inclination,"
	"ascending,"
	"perihelion,"
	"anomaly,"
	"motion,"
	"epoch,"
	"type,"
	"designation,"
	"observations,"
	"last,"
	"H,"
	"G)"
	"VALUES("
	":name,"
	":eccentricity,"
	":semimajor,"
	":inclination,"
	":ascending,"
	":perihelion,"
	":anomaly,"
	":motion,"
	":epoch,"
	":type,"
	":designation,"
	":observations,"
	":last,"
	":H,"
	":G);";

    query.prepare(prepare);

    query.bindValue(":name",         QVariant(asteroid.designation_readable));
    query.bindValue(":eccentricity", QVariant(asteroid.eccentricity));
    query.bindValue(":semimajor",    QVariant(asteroid.semimajor_axis));
    query.bindValue(":inclination",  QVariant(asteroid.inclination));
    query.bindValue(":ascending",    QVariant(asteroid.ascending_node));
    query.bindValue(":perihelion",   QVariant(asteroid.perihelion));
    query.bindValue(":anomaly",      QVariant(asteroid.anomaly_mean));
    query.bindValue(":motion",       QVariant(asteroid.daily_motion));
    query.bindValue(":epoch",        QVariant(asteroid.epoch));
    query.bindValue(":type",         QVariant(asteroid.classification_flag));
    query.bindValue(":designation",  QVariant(asteroid.designation));
    query.bindValue(":observations", QVariant(asteroid.observations));
    query.bindValue(":last",         QVariant(asteroid.last_observation));
    query.bindValue(":H",            QVariant(asteroid.H));
    query.bindValue(":G",            QVariant(asteroid.G));

    if (!query.exec()) {
        qWarning() << query.lastError().text();
        db.exec("END TRANSCATION");
        return false;
    }

    db.exec("END TRANSCATION");
    return true;
}

bool Sqlite::addEntryComet(const comet_t &comet)
{
    QSqlDatabase db = getDb();

    if (!db.isOpen())
        return false;

    /* Significant speed up, using PRAGMA and TRANSCATION. */
    db.exec("PRAGMA synchronous = OFF");
    db.exec("PRAGMA Journal_mode = MEMORY");
    db.exec("BEGIN TRANSCATION");

    QSqlQuery query(db);
    const QString prepare =
	"REPLACE INTO " TABLE_COMET "("
	"name,"
	"eccentricity,"
	"inclination,"
	"ascending,"
	"perihelion,"
	"perihelion_year,"
	"perihelion_month,"
	"perihelion_day,"
	"perihelion_distance,"
	"type,"
	"H,"
	"G)"
	"VALUES("
	":name,"
	":eccentricity,"
	":inclination,"
	":ascending,"
	":perihelion,"
	":perihelion_year,"
	":perihelion_month,"
	":perihelion_day,"
	":perihelion_distance,"
	":type,"
	":H,"
	":G);";

    query.prepare(prepare);

    query.bindValue(":name",                QVariant(comet.designation_name));
    query.bindValue(":eccentricity",        QVariant(comet.eccentricity));
    query.bindValue(":inclination",         QVariant(comet.inclination));
    query.bindValue(":ascending",           QVariant(comet.ascending_node));
    query.bindValue(":perihelion",          QVariant(comet.perihelion));
    query.bindValue(":perihelion_year",     QVariant(comet.perihelion_year));
    query.bindValue(":perihelion_month",    QVariant(comet.perihelion_month));
    query.bindValue(":perihelion_day",      QVariant(comet.perihelion_day));
    query.bindValue(":perihelion_distance", QVariant(comet.perihelion_distance));
    query.bindValue(":type",                QVariant(comet.orbit_type));
    query.bindValue(":H",                   QVariant(comet.H));
    query.bindValue(":G",                   QVariant(comet.G));

    if (!query.exec()) {
        qWarning() << query.lastError().text();
        db.exec("END TRANSCATION");
        return false;
    }

    db.exec("END TRANSCATION");
    return true;
}

bool Sqlite::writeSetting(const QString &key, const QVariant &value, const QString &filenameDb)
{
    return writeSettings(QMap<QString, QVariant> { { key, value } }, filenameDb);
}

bool Sqlite::writeSettings(const QMap<QString, QVariant> &map, const QString &filenameDb)
{
    Sqlite connection(filenameDb);
    QSqlDatabase db = connection.getDb();

    if (!db.isOpen())
        return false;

    QSqlQuery query(db);

    query.prepare("REPLACE INTO " TABLE_SETTING "(key, value) VALUES(?, ?)");
    query.addBindValue(QVariant(map.keys()));
    query.addBindValue(map.values());

    if (!query.execBatch()) {
        qWarning() << query.lastError().databaseText();
        return false;
    }

    return true;
}

QVariant Sqlite::readSetting(const QString &key, const QString &filenameDb)
{
    QMap<QString, QVariant> map { { key, "" } };
    readSettings(map, filenameDb);

    return map[key];
}

bool Sqlite::readSettings(QMap<QString, QVariant> &map, const QString &filenameDb)
{
    Sqlite connection(filenameDb);
    QSqlDatabase db = connection.getDb();

    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    const QString sqlQuery = "SELECT key, value FROM " TABLE_SETTING ";";

    if (!query.exec(sqlQuery)) {
        qWarning() << "Sql query" << sqlQuery << "failed" << query.lastError().databaseText();
        return false;
    }

    while (query.next()) {
        QString key = query.record().value("key").toString();
        QVariant value = query.record().value("value");
        map[key] = value;
    }

    return true;
}
