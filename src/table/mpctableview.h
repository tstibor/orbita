#pragma once

#include <QTableView>
#include <QSqlQueryModel>
#include "common.h"

enum MpcType : quint8 { ASTEROID = 0, COMET = 1, NEO = 2};

class MpcTableView : public QTableView
{
    Q_OBJECT

public:
    explicit MpcTableView(QString cmd, MpcType celestialType);
    ~MpcTableView();

    QSqlQueryModel *attachedSqlQueryModel();
    void setSqlQueryCmd(QString cmd) { m_SqlQueryCmd = cmd; };
    const QString SqlQueryCmd() { return m_SqlQueryCmd; };
    const MpcType celestialType() { return m_CelestialType; };

private:
    QSqlQueryModel *m_SqlQueryModel;
    QString m_SqlQueryCmd;
    MpcType m_CelestialType;
};
