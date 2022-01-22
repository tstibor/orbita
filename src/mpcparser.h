#ifndef MPCPARSER_H
#define MPCPARSER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QDebug>
#include <libnova/julian_day.h>
#include "types.h"

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

    bool parseAsteroid(const QString &line, struct asteroid_t *asteroid);
    bool parseComet(const QString &line, struct comet_t *comet);

signals:
    void parsedAsteroid(const struct asteroid_t &asteroid);
    void parsedComet(const struct comet_t &comet);
    void progress(qreal percent, quint32 linesProcessed);
    void finished();

public slots:
    void abort();
};

#endif // MPCPARSER_H
