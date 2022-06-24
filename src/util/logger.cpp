#include "logger.h"

Logger *Logger::instance()
{
    static Logger inst;

    return &inst;
}

void Logger::create(QString filename)
{
    if (m_logFile)
	close();

    m_logFile = new QFile(filename);
    m_logFileIsOpen = m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    if (m_logFileIsOpen)
	qDebug() << "logging messages to file" << filename;
    else
	qWarning() << "logging messages to file" << filename << "failed:" << m_logFile->errorString();
}

void Logger::close()
{
    if (m_logFile != nullptr) {
	m_logFile->close();
	m_logFileIsOpen = false;
    }

    delete m_logFile;
    m_logFile = nullptr;
}

Logger::Logger(QObject *parent) : QObject(parent)
{
#ifndef QT_DEBUG
    QLoggingCategory::setFilterRules("*.debug=false");
#endif

    qInstallMessageHandler(messageHandler);
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz "));

    switch(type) {
    case QtDebugMsg:
	text.append("DEBUG ").append(QString("%1:%2↷%3")
				     .arg(QFileInfo(context.file).baseName())
				     .arg(context.line)
				     .arg(context.function)).append("] ");
	break;
    case QtInfoMsg:
	text.append("INFO    ] ");
	break;
    case QtWarningMsg:
	text.append("WARNING ] ");
	break;
    case QtCriticalMsg:
	text.append("CRITICAL] ");
	break;
    case QtFatalMsg:
	text.append("FATAL   ] ");
    }
    text.append(msg);

    QTextStream(stderr) << text << Qt::endl;

    if (Logger::instance()->m_logFileIsOpen) {
	QTextStream(Logger::instance()->m_logFile) << text << Qt::endl;
	Logger::instance()->m_logFile->flush();
    }
}
