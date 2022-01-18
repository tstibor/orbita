#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtCore>
#include <QtNetwork>

class DownloadManager: public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    void start(const QStringList &StringListUrl);

signals:
    void progress(const QString &filename, const QElapsedTimer &elapsedTimer, qint64 bytesReceived, qint64 bytesTotal);
    void finished(const QString &filename);
    void finishedAll();

private slots:
    void startNextDownload();
    void downloadFinished();
    void downloadReadyRead();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkReply *m_NetworkReply = nullptr;
    QNetworkAccessManager m_NetworkAccessManager;
    QQueue<QUrl> m_Queue;
    QFile m_File;
    QElapsedTimer m_ElapsedTimer;

    QString saveFileName(const QUrl &url);
};

#endif
