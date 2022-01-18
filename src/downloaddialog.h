#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QFontMetrics>
#include <QProgressBar>
#include <QUrl>
#include "common.h"
#include "downloadmanager.h"

class DownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog();

private slots:
    void progress(const QString &filename, const QElapsedTimer &elapsedTimer,
		  qint64 bytesReceived, qint64 bytesTotal);

signals:
    void fileDownloaded(const QString &filename);

private:
    QVBoxLayout *m_VBoxLayout = nullptr;
    QHBoxLayout *m_HBoxLayout = nullptr;
    QFormLayout *m_FormLayout = nullptr;
    QLineEdit *m_LineEditAsteroids = nullptr;
    QLineEdit *m_LineEditComets = nullptr;
    QCheckBox *m_CheckBoxAsteroids = nullptr;
    QCheckBox *m_CheckBoxComets = nullptr;
    QPushButton *m_PushButtonDownload = nullptr;
    QPushButton *m_PushButtonClose = nullptr;
    QProgressBar *m_ProgressBar = nullptr;
    DownloadManager *m_DownloadManager = nullptr;
    QStringList m_StringListUrls;
    QStringList m_StringListFileNames;

    void connectActions();
};

#endif /* DOWNLOADDIALOG_H */
