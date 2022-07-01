#ifndef TESTMPCPARSER_H
#define TESTMPCPARSER_H

#include <QTest>
#include "settings.h"
#include "mpcparser.h"
#include "sqlite.h"
#include "common.h"

#define MPC_ASTEROID_FILE       ":/res/test/MPCORB.DAT"
#define MPC_ASTEROID_FILE_GZ    ":/res/test/MPCORB.DAT.gz"
#define MPC_COMET_FILE          ":/res/test/CometEls.txt"

class TestMpcParser : public QObject
{
    Q_OBJECT

private:
    QString m_filenameDb;
    QTextStream *m_textStream = nullptr;

    void parseMpcTest(QString filename, OrbType orbType)
    {
	MpcParser mpcParser(filename, orbType);
	QFile inputFile(filename);
	quint32 count_parsed = 0;
	const bool isCompressed = QFileInfo(filename.toUpper()).suffix() == "GZ";

	connect(&mpcParser, &MpcParser::parsedAsteroid,
		[&count_parsed]() { count_parsed++; });

	if (isCompressed)
	    inputFile.open(QIODevice::ReadOnly);
	else
	    inputFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QVERIFY(inputFile.isOpen());


	QByteArray outDecompressed;
	QByteArray inCompressed;

	if (isCompressed) {
	    inCompressed = inputFile.readAll();
	    int rc = decompressGzip(outDecompressed, inCompressed);

	    if (rc)
		inputFile.close();

	    QCOMPARE(rc, 0);
	}

	QVERIFY(mpcParser.start());

	if (isCompressed)
	    m_textStream = new QTextStream(outDecompressed);
	else
	    m_textStream = new QTextStream(&inputFile);

	QString line;
	quint32 count = 0;

	/* Actual data starts after "----------"... before is header. */
	while (!line.startsWith("----------") && m_textStream->readLineInto(&line));
	while (m_textStream->readLineInto(&line)) {
	    if (!line.isEmpty())
		count++;
	}

	inputFile.close();

	QCOMPARE(count, count_parsed);
	SAFE_DELETE(m_textStream);
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
	parseMpcTest(MPC_ASTEROID_FILE_GZ, OrbType::ASTEROID);
    }

    void parseMpcCometTest()
    {
	parseMpcTest(MPC_COMET_FILE, OrbType::COMET);
    }

    void cleanupTestCase()
    {
	SAFE_DELETE(m_textStream);
    }
};

#endif // TESTMPCPARSER_H
