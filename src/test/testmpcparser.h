#ifndef TESTMPCPARSER_H
#define TESTMPCPARSER_H

#include <QTest>
#include "settings.h"
#include "mpcparser.h"
#include "sqlite.h"

#define MPC_ASTEROID_FILE ":/res/test/MPCORB.DAT"
#define MPC_COMET_FILE    ":/res/test/CometEls.txt"

class TestMpcParser : public QObject
{
    Q_OBJECT

private:
    QString m_filenameDb;

    void parseMpcTest(QString filename, OrbType orbType)
    {
	MpcParser mpcParser(filename, orbType);
	QFile inputFile(filename);
	quint32 count_parsed = 0;

	connect(&mpcParser, &MpcParser::parsedAsteroid, [&count_parsed]() { count_parsed++; });

	inputFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QVERIFY(inputFile.isOpen());

	QVERIFY(mpcParser.start());

	QTextStream textStream(&inputFile);
	QString line;
	quint32 count = 0;

	/* Actual data starts after "----------"... before is header. */
	while (!line.startsWith("----------") && textStream.readLineInto(&line));
	while (textStream.readLineInto(&line)) {
	    if (!line.isEmpty())
		count++;
	}

	inputFile.close();

	QCOMPARE(count, count_parsed);
    }

private slots:
    void initTestCase()
    {
	Settings::instance();

        const QString tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        m_filenameDb = tempLocation + "/" + QCoreApplication::applicationName() + ".db";
    }

    void parseMpcAsteroidTest()
    {
	parseMpcTest(MPC_ASTEROID_FILE, OrbType::ASTEROID);
    }

    void parseMpcCometTest()
    {
	parseMpcTest(MPC_COMET_FILE, OrbType::COMET);
    }

    void cleanupTestCase()
    {
    }
};

#endif // TESTMPCPARSER_H
