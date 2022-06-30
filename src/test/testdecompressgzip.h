#ifndef TESTDECOMPRESSGZIP_H
#define TESTDECOMPRESSGZIP_H

#include <QTest>
#include "decompress.h"

#define MPC_ASTEROID_FILE_GZ ":/res/test/MPCORB.DAT.gz"
#define MPC_ASTEROID_FILE    ":/res/test/MPCORB.DAT"

class TestDecompressGzip : public QObject
{
    Q_OBJECT

private:
    QFile m_fileCompressed{MPC_ASTEROID_FILE_GZ};
    QFile m_fileDecompressed{MPC_ASTEROID_FILE};

private slots:
    void initTestCase()
    {
	m_fileCompressed.open(QIODevice::ReadOnly);
	m_fileDecompressed.open(QIODevice::ReadOnly);

	QVERIFY(m_fileCompressed.isOpen());
	QVERIFY(m_fileDecompressed.isOpen());
    }

    void decompressGzipTest()
    {
	QByteArray outDecompressed;
	QByteArray inCompressed;
	QByteArray origContent;

	inCompressed = m_fileCompressed.readAll();
	origContent = m_fileDecompressed.readAll();

	int rc = decompressGzip(outDecompressed, inCompressed);

	QCOMPARE(rc, 0);
	QCOMPARE(outDecompressed, origContent);
    }

    void cleanupTestCase()
    {
	m_fileCompressed.close();
	m_fileDecompressed.close();
    }
};

#endif	/* TESTDECOMPRESSGZIP_H */
