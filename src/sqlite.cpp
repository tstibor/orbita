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

bool Sqlite::addEntryAsteroid(const QVariantList &orbEntry)
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

    query.bindValue(":name",         orbEntry.at(0));
    query.bindValue(":eccentricity", orbEntry.at(1));
    query.bindValue(":semimajor",    orbEntry.at(2));
    query.bindValue(":inclination",  orbEntry.at(3));
    query.bindValue(":ascending",    orbEntry.at(4));
    query.bindValue(":perihelion",   orbEntry.at(5));
    query.bindValue(":anomaly",      orbEntry.at(6));
    query.bindValue(":motion",       orbEntry.at(7));
    query.bindValue(":epoch",        orbEntry.at(8));
    query.bindValue(":type",         orbEntry.at(9));
    query.bindValue(":designation",  orbEntry.at(10));
    query.bindValue(":observations", orbEntry.at(11));
    query.bindValue(":last",         orbEntry.at(12));
    query.bindValue(":H",            orbEntry.at(13));
    query.bindValue(":G",            orbEntry.at(14));

    if (!query.exec()) {
        qWarning() << query.lastError().text();
        db.exec("END TRANSCATION");
        return false;
    }

    db.exec("END TRANSCATION");
    return true;
}

bool Sqlite::addEntryComet(const QVariantList &orbEntry)
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

    query.bindValue(":name",                orbEntry.at(0));
    query.bindValue(":eccentricity",        orbEntry.at(1));
    query.bindValue(":inclination",         orbEntry.at(2));
    query.bindValue(":ascending",           orbEntry.at(3));
    query.bindValue(":perihelion",          orbEntry.at(4));
    query.bindValue(":perihelion_year",     orbEntry.at(5));
    query.bindValue(":perihelion_month",    orbEntry.at(6));
    query.bindValue(":perihelion_day",      orbEntry.at(7));
    query.bindValue(":perihelion_distance", orbEntry.at(8));
    query.bindValue(":type",                orbEntry.at(9));
    query.bindValue(":H",                   orbEntry.at(10));
    query.bindValue(":G",                   orbEntry.at(11));

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
