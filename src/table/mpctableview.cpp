#include "mpctableview.h"

MpcTableView::MpcTableView(QString cmd, MpcType celestialType)
{
    m_CelestialType = celestialType;
    setSqlQueryCmd(cmd);
    m_SqlQueryModel = new QSqlQueryModel(this);
}

MpcTableView::~MpcTableView()
{
    SAFE_DELETE(m_SqlQueryModel);
}

QSqlQueryModel *MpcTableView::attachedSqlQueryModel()
{
    return m_SqlQueryModel;
}
