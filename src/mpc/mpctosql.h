#ifndef MPCTOSQL_H
#define MPCTOSQL_H

#include <QObject>
#include "sqlite.h"
#include "mpcparser.h"
#include "progressdialog.h"

class MpcToSql : public QObject
{
    Q_OBJECT

public:
    explicit MpcToSql(QObject *parent = nullptr);
    ~MpcToSql();
    void processFile(const QString &filename, OrbType orbType);

signals:
    void finished(const QString &filename);

private:
    Sqlite *m_Sqlite = nullptr;
};

#endif /* MPCTOSQL_H */
