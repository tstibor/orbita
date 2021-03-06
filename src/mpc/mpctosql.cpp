#include "mpctosql.h"
#include "mpcparser.h"

MpcToSql::MpcToSql(QObject *parent) : QObject(parent)
{
    m_Sqlite = new Sqlite { Settings::FilenameDb(), "mpcparser" };
}

MpcToSql::~MpcToSql()
{
    if (m_Sqlite) {
	delete m_Sqlite;
	m_Sqlite = nullptr;
    }
}

void MpcToSql::processFile(const QString &filename, OrbType orbType)
{
    QThread *m_ThreadMpcParser = new QThread();
    MpcParser *m_MpcParser = new MpcParser(filename, orbType);

    ProgressDialog *progressDialog = new ProgressDialog();
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowTitle(QFileInfo { filename }.fileName());
    progressDialog->show();

    connect(m_MpcParser, &MpcParser::parsedAsteroid,
	    m_Sqlite, &Sqlite::addEntryAsteroid, Qt::BlockingQueuedConnection);
    connect(m_MpcParser, &MpcParser::parsedComet,
	    m_Sqlite, &Sqlite::addEntryComet, Qt::BlockingQueuedConnection);
    connect(m_MpcParser, &MpcParser::progress, progressDialog, &ProgressDialog::progress);
    connect(m_MpcParser, &MpcParser::finished, [&](const QString &filename) {
	emit finished(filename);
    });

    /* Cancel signal terminates read loop in MpcParser and emit there a finish signal. */
    connect(progressDialog, &QProgressDialog::canceled, m_MpcParser, &MpcParser::abort,
            Qt::DirectConnection);
    connect(m_ThreadMpcParser, &QThread::started,  m_MpcParser, &MpcParser::start);

    /* Cleanup. */
    connect(m_MpcParser, &MpcParser::finished, progressDialog, &ProgressDialog::deleteLater);
    connect(m_MpcParser, &MpcParser::finished, m_ThreadMpcParser, &QThread::quit);
    connect(m_MpcParser, &MpcParser::finished, m_MpcParser, &MpcParser::deleteLater);
    connect(m_ThreadMpcParser, &QThread::finished, m_ThreadMpcParser, &QThread::deleteLater);

    m_MpcParser->moveToThread(m_ThreadMpcParser);
    m_ThreadMpcParser->start();
}
