#ifndef MPCPARSER_H
#define MPCPARSER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QFileInfo>
#include <libnova/julian_day.h>
#include "types.h"
#include "common.h"
#include "decompress.h"

#define MPCORB_HEADER_SIZE 2349

enum class OrbType { ASTEROID, COMET, UNKNOWN };

class MpcParser : public QObject
{
    Q_OBJECT

public:
    MpcParser(QString filename, OrbType orbType);
    ~MpcParser();
    bool start();

private:
    QString m_filename;
    bool m_abort;
    OrbType m_orbType;
    QTextStream *m_textStream;
    QFile *m_file;

    bool isFileCompressed();
    quint64 fileAsTextStream();
    bool parseAsteroid(const QString &line, struct asteroid_t *asteroid);
    bool parseComet(const QString &line, struct comet_t *comet);

signals:
    void parsedAsteroid(const struct asteroid_t &asteroid);
    void parsedComet(const struct comet_t &comet);
    void progress(qreal percent, quint32 linesProcessed);
    void finished(const QString &filename);

public slots:
    void abort();
};

#endif // MPCPARSER_H
