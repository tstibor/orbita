#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QSqlQueryModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QKeyEvent>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include "libnova/julian_day.h"
#include "libnova/asteroid.h"
#include "libnova/elliptic_motion.h"
#include "sqlite.h"
#include "settings.h"
#include "asteroid.h"
#include "solarsystem.h"

class TableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableDialog(QWidget *parent = nullptr, SolarSystem *solarSystem = nullptr);
    ~TableDialog();

protected:
    SolarSystem &m_SolarSystem;
    QTableView *m_TableView;

    bool eventFilter(QObject *object, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void reject() override;
    virtual void renderSelection() {};

    void setSqlCmdQuery(const QString &sqlCmdQuery) { m_SqlCmdQuery = sqlCmdQuery; }
    void setSqlCmdNumRow(const QString &sqlCmdNumRow) { m_SqlCmdNumRow = sqlCmdNumRow; }
    void setOrbType(OrbType orbType) { m_OrbType = orbType; }
    void setLineEditText(const QString &text) { m_LineEdit->setText(text); }

private:
    QSqlQueryModel *m_SqlQueryModel;
    QSortFilterProxyModel *m_SortFilterProxyModel;
    QVBoxLayout *m_VBoxLayout;
    QHBoxLayout *m_HBoxLayout;
    QPushButton *m_PushButtonDisplay;
    QPushButton *m_PushButtonClear;
    QPushButton *m_PushButtonExecQuery;
    QLineEdit *m_LineEdit;

    quint32 m_totalObjects;
    QString m_SqlCmdQuery;
    QString m_SqlCmdNumRow;
    OrbType m_OrbType;

    void createConnections();
    void clickedButtonDisplay();
    void clickedButtonExecQuery();
    void updateWindowTitle(quint32 numSelectedObjects = 0);

public slots:
    void showQueryInTable();
};

#endif // TABLEDIALOG_H
