#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{
}

void DownloadManager::start(const QStringList &StringListUrl)
{
    for (const QString &urlAsString : StringListUrl) {

	if (m_Queue.isEmpty())
	    QTimer::singleShot(0, this, &DownloadManager::startNextDownload);

	m_Queue.enqueue(QUrl::fromEncoded(urlAsString.toLocal8Bit()));
    }
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    return QFileInfo(url.path()).fileName();
}

void DownloadManager::startNextDownload()
{
    if (m_Queue.isEmpty()) {
	emit finishedAll();
        return;
    }

    QUrl url = m_Queue.dequeue();
    QString filename = saveFileName(url);

    m_File.setFileName(filename);

    if (!m_File.open(QIODevice::WriteOnly)) {
        qWarning() << qPrintable(filename) << url.toEncoded().constData() << qPrintable(m_File.errorString());
        startNextDownload();

        return;
    }

    QNetworkRequest request(url);
    m_NetworkReply = m_NetworkAccessManager.get(request);

    connect(m_NetworkReply, &QNetworkReply::finished, this, &DownloadManager::downloadFinished);
    connect(m_NetworkReply, &QNetworkReply::readyRead, this, &DownloadManager::downloadReadyRead);
    connect(m_NetworkReply, &QNetworkReply::downloadProgress, this, &DownloadManager::downloadProgress);

    m_ElapsedTimer.start();
}

void DownloadManager::downloadFinished()
{
    m_File.close();

    if (m_NetworkReply->error()) {
	qWarning() << "Downloading failed: " << qPrintable(m_NetworkReply->errorString());
	m_File.remove();
    } else
	emit finished(m_File.fileName());

    m_NetworkReply->deleteLater();
    startNextDownload();
}

void DownloadManager::downloadReadyRead()
{
    m_File.write(m_NetworkReply->readAll());
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit progress(m_File.fileName(), m_ElapsedTimer, bytesReceived, bytesTotal);
}
