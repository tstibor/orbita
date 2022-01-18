#include "downloaddialog.h"

DownloadDialog::DownloadDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Download MPC files"));

    m_DownloadManager = new DownloadManager(this);

    m_VBoxLayout = new QVBoxLayout;
    m_HBoxLayout = new QHBoxLayout;
    m_FormLayout = new QFormLayout;

    m_CheckBoxAsteroids = new QCheckBox;
    m_CheckBoxComets = new QCheckBox;
    m_LineEditAsteroids = new QLineEdit(DEFAULT_DOWNLOAD_MPCORB_DAT_GZ_URL);
    m_LineEditComets = new QLineEdit(DEFAULT_DOWNLOAD_COMET_ELS_URL);
    m_PushButtonDownload = new QPushButton(tr("Download"));
    m_PushButtonClose = new QPushButton(tr("Close"));
    m_ProgressBar = new QProgressBar;

    m_LineEditAsteroids->setMinimumWidth(10 +
					 QFontMetrics(QApplication::font()).
					 horizontalAdvance(DEFAULT_DOWNLOAD_MPCORB_DAT_GZ_URL));
    m_LineEditComets->setMinimumWidth(10 +
				      QFontMetrics(QApplication::font()).
				      horizontalAdvance(DEFAULT_DOWNLOAD_COMET_ELS_URL));

    m_FormLayout->addRow(m_CheckBoxAsteroids, m_LineEditAsteroids);
    m_FormLayout->addRow(m_CheckBoxComets, m_LineEditComets);

    m_VBoxLayout->addLayout(m_FormLayout);
    m_VBoxLayout->addWidget(m_ProgressBar);
    m_HBoxLayout->addWidget(m_PushButtonDownload);
    m_HBoxLayout->addWidget(m_PushButtonClose);
    m_VBoxLayout->addLayout(m_HBoxLayout);

    setLayout(m_VBoxLayout);

    connectActions();
}

DownloadDialog::~DownloadDialog()
{
    if (m_DownloadManager) {
	delete m_DownloadManager;
	m_DownloadManager = nullptr;
    }
}

void DownloadDialog::connectActions()
{
    connect(m_DownloadManager, &DownloadManager::progress, this, &DownloadDialog::progress);
    connect(m_DownloadManager, &DownloadManager::finished, [&](const QString &filename) {
	emit fileDownloaded(filename);
    });

    connect(m_DownloadManager, &DownloadManager::finishedAll, [&] {
	m_StringListFileNames.clear();
	m_StringListUrls.clear();
	m_PushButtonDownload->setDisabled(false);
    });

    connect(m_PushButtonDownload, &QPushButton::pressed, [&] {

  	if (m_CheckBoxAsteroids->checkState() == Qt::CheckState::Checked)
	    m_StringListUrls.append(m_LineEditAsteroids->text());

  	if (m_CheckBoxComets->checkState() == Qt::CheckState::Checked)
	    m_StringListUrls.append(m_LineEditComets->text());

	if (m_StringListUrls.isEmpty())
	    return;

	m_PushButtonDownload->setDisabled(true);
	m_DownloadManager->start(m_StringListUrls);
    });

    connect(m_PushButtonClose, &QPushButton::pressed, this, [&]{
	close();
    });
}

void DownloadDialog::progress(const QString &filename, const QElapsedTimer &elapsedTimer,
			      qint64 bytesReceived, qint64 bytesTotal)
{
    double speed = bytesReceived * 1000.0 / elapsedTimer.elapsed();
    QString unit;

    if (speed < 1024) {
	unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
	speed /= 1024;
	unit = "kB/s";
    } else {
	speed /= 1024 * 1024;
	unit = "MB/s";
    }

    m_ProgressBar->setMaximum(bytesTotal);
    m_ProgressBar->setValue(bytesReceived);
    m_ProgressBar->setFormat("Speed: "
			     + QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit)
			     + "  Filename: "
			     + filename);
}
