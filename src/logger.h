#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QLoggingCategory>

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger* instance();
    void create(QString filename);
    void close();

private:
    explicit Logger(QObject *parent = nullptr);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    QFile *m_logFile = nullptr;
    bool m_logFileIsOpen = false;
};

#endif // LOGGER_H
