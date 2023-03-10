#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QTableView>
#include <QSqlQueryModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QKeyEvent>
#include "solarsystem.h"
#include "sqlite.h"
#include "settings.h"
#include "mpctableview.h"

enum class MpcTable { MPC_ASTEROIDS = 0, MPC_COMETS = 1 };

class MpcTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MpcTableDialog(QWidget *parent = nullptr, SolarSystem *solarSystem = nullptr);

protected:
    void setSqlCmdQuery(const QString &sqlCmdQuery) { m_SqlCmdQuery = sqlCmdQuery; }
    void setLineEditText(const QString &text) { m_LineEdit->setText(text); }
    bool eventFilter(QObject *object, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    SolarSystem &m_SolarSystem;

    QTabWidget *m_TabWidget = nullptr;
    MpcTableView *m_TableViewAsteroids = nullptr;
    MpcTableView *m_TableViewComets = nullptr;

    QVBoxLayout *m_VBoxLayout = nullptr;
    QHBoxLayout *m_HBoxLayout = nullptr;
    QHBoxLayout *m_HBoxLayoutQueryCmd = nullptr;
    QPushButton *m_PushButtonDisplay = nullptr;
    QPushButton *m_PushButtonClear = nullptr;
    QPushButton *m_PushButtonExecQuery = nullptr;
    QLabel *m_LabelQueryCmd = nullptr;
    QLineEdit *m_LineEdit = nullptr;

    QString m_SqlCmdQuery;

    void connectSignalsToSlots();
    void prepareQueryForTable(MpcTable table);
    void prepareOpen();
    void renderSelectedAsteroids();
    void renderSelectedComets();

private slots:
    void updateWindowTitle(quint32 selected, quint32 total);
    void clickedButtonExecQuery();

public slots:
    void queryResultsToTableView();
    void openAndShow();

signals:
    void numberRows(quint32 selected, quint32 total);
    void queryStarted();
    void queryFinished();
    void selectedAsteroid(struct asteroid_t &asteroid);
    void displaySelectedAsteroids();
    void selectedComet(struct comet_t &comet);
    void displaySelectedComets();
};
