#pragma once

#include <QTableView>
#include <QSqlQueryModel>
#include "common.h"

enum class CelestialType { ASTEROID, COMET, NEO };

class MpcTableView : public QTableView
{
    Q_OBJECT

public:
    explicit MpcTableView(QString cmd, CelestialType celestialType);
    ~MpcTableView();

    QSqlQueryModel *attachedSqlQueryModel();
    void setSqlQueryCmd(QString cmd) { m_SqlQueryCmd = cmd; };
    const QString SqlQueryCmd() { return m_SqlQueryCmd; };
    const CelestialType celestialType() { return m_CelestialType; };

private:
    QSqlQueryModel *m_SqlQueryModel;
    QString m_SqlQueryCmd;
    CelestialType m_CelestialType;
};
