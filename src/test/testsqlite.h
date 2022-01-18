#ifndef TESTSQLITE_H
#define TESTSQLITE_H

#include <QTest>
#include "settings.h"
#include "sqlite.h"

class TestSqlite : public QObject
{
    Q_OBJECT

private:
    QString m_filenameDb;

private slots:
    void initTestCase()
    {
	Settings::instance();

	const QString tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
	m_filenameDb = tempLocation + "/" + QCoreApplication::applicationName() + ".db";
    }

    void dbFilenameTest()
    {
	const QString appLocationData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	const QString FilenameDb = appLocationData + "/" + QCoreApplication::applicationName() + ".db";

	QCOMPARE(FilenameDb, Settings::FilenameDb());
    }

    void createTest0()
    {
	Sqlite *connection0 = new Sqlite(Settings::FilenameDb());
	{
	    QSqlDatabase db = connection0->getDb();
	    QVERIFY(db.isOpen());
	}
	delete connection0;
    }

    void createTest1()
    {
	Sqlite *connection1 = new Sqlite(m_filenameDb);
	{
	    QSqlDatabase db = connection1->getDb();

	    QVERIFY(db.isOpen());
	    QVERIFY(connection1->createAllTables());
	}
	delete connection1;
    }

    void settingsTest0()
    {
	Sqlite::writeSetting("setting1", "value1", m_filenameDb);
	QCOMPARE(Sqlite::readSetting("setting1", m_filenameDb), "value1");

	Sqlite::writeSetting("setting2", "value2", m_filenameDb);
	QCOMPARE(Sqlite::readSetting("setting2", m_filenameDb), "value2");

	Sqlite::writeSetting("setting3", 3.1415, m_filenameDb);
	QCOMPARE(Sqlite::readSetting("setting3", m_filenameDb), 3.1415);

	Sqlite::writeSetting("setting4", QByteArray("235711131719"), m_filenameDb);;
	QCOMPARE(Sqlite::readSetting("setting4", m_filenameDb), QByteArray("235711131719"));

	Sqlite::writeSetting("setting1", "value2", m_filenameDb);
	QCOMPARE(Sqlite::readSetting("setting1", m_filenameDb), "value2");
    }

    void settingsTest1()
    {
	QCOMPARE(Sqlite::readSetting("setting1", m_filenameDb), "value2");
	QCOMPARE(Sqlite::readSetting("setting3", m_filenameDb), 3.1415);
	QCOMPARE(Sqlite::readSetting("setting4", m_filenameDb), QByteArray("235711131719"));
    }

    void cleanupTestCase()
    {
    }
};

#endif // TESTSQLITE_H
