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
#include <QMenu>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCheckBox>
#include <QSlider>
#include "solarsystem.h"
#include "sqlite.h"
#include "settings.h"
#include "mpctableview.h"
#include "renderwindow.h"

class MpcTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MpcTableDialog(QWidget *parent = nullptr,
			    SolarSystem *solarSystem = nullptr);

protected:
    void setSqlCmdQuery(const QString &sqlCmdQuery) { m_SqlCmdQuery = sqlCmdQuery; }
    void setLineEditText(const QString &text) { m_LineEdit->setText(text); }
    bool eventFilter(QObject *object, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    SolarSystem &m_SolarSystem;

    QTabWidget *m_TabWidget { nullptr };
    MpcTableView *m_TableViewAsteroids { nullptr };
    MpcTableView *m_TableViewComets { nullptr };

    QVBoxLayout *m_VBoxLayout { nullptr };
    QHBoxLayout *m_HBoxLayoutQueryCmd { nullptr };
    QPushButton *m_PushButtonDisplay { nullptr };
    QPushButton *m_PushButtonClear { nullptr };
    QGridLayout *m_GridLayoutSettings { nullptr };
    QLabel *m_LabelDisplayOptions { nullptr };
    QCheckBox *m_CheckBoxOrbit { nullptr };
    QCheckBox *m_CheckBoxName { nullptr };
    QCheckBox *m_CheckBoxDate { nullptr };
    QCheckBox *m_CheckBoxMag { nullptr };
    QCheckBox *m_CheckBoxDist { nullptr };

    QLabel *m_LabelQueryCmd { nullptr };
    QLineEdit *m_LineEdit { nullptr };

    QLabel *m_LabelSlider { nullptr };
    QSlider *m_SliderRadius = { nullptr };
    int m_sliderValue[2] = { 50, 50 };

    quint16 m_displayOptions[2] = { 0, 0 };

    QString m_SqlCmdQuery;

    void connectSignalsToSlots();
    void prepareQueryForTable(MpcType type);
    void prepareOpen();
    void renderSelectedAsteroids();
    void renderSelectedComets();
    void updateCheckBoxes(int index);
    void updateSlider();
    void switchOptions(int state, quint16 optionBit);
    void switchSliderValue(int value);

private slots:
    void updateWindowTitle(quint32 selected, quint32 total);

public slots:
    void queryResultsToTableView();
    void openAndShow();

signals:
    void numberRows(quint32 selected, quint32 total);
    void queryStarted();
    void queryFinished();
    void selectedAsteroid(struct asteroid_t &asteroid);
    void selectedComet(struct comet_t &comet);
    void displaySelection(MpcType type);
    void updateDisplayOptions(quint16 optionBit, MpcType type);
    void updateSliderValue(int value, MpcType type);
};
