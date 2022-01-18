#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QPixmap>
#include <QLabel>
#include <QComboBox>
#include <QDateTimeEdit>
#include "solarsystem.h"
#include "renderwindow.h"
#include "statusbarwidget.h"
#include "sqlite.h"
#include "mpcparser.h"
#include "mpctosql.h"
#include "progressdialog.h"
#include "tabledialog.h"
#include "asteroidtabledialog.h"
#include "comettabledialog.h"
#include "downloaddialog.h"
#include "clickablelabel.h"
#include "version.h"

enum class TimeDirection {INCREMENT, DECREMENT};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void updateRenderSettingAsteroid(quint16 fields);
    void updateRenderSettingComet(quint16 fields);

private slots:
    void about();

private:
    RenderWindow *m_RenderWindow = nullptr;
    StatusbarWidget *m_StatusbarWidget = nullptr;
    SolarSystem *m_SolarSystem = nullptr;
    AsteroidTableDialog *m_AsteroidTableDialog = nullptr;
    CometTableDialog *m_CometTableDialog = nullptr;
    MpcToSql *m_MpcToSql = nullptr;

    QDateTimeEdit *m_DateTimeEdit = nullptr;
    QPushButton *m_PushButtonIncrTime = nullptr;
    QPushButton *m_PushButtonDecrTime = nullptr;
    QSpinBox *m_SpinBoxIncrDecrTime = nullptr;
    QComboBox *m_ComboBoxIncrDecrTime = nullptr;
    QSlider *m_SliderRadiusAsteroids = nullptr;
    QLabel *m_LabelPlanet = nullptr;
    QCheckBox *m_CheckBoxRadiusPlanetsSun = nullptr;
    QSlider *m_SliderRadiusPlanetsSun = nullptr;
    QComboBox *m_ComboBoxRenderSettings = nullptr;
    QStandardItemModel *m_StandardItemModel = nullptr;
    QList<quint8> m_QListComboBoxRows;
    quint16 m_ComboBoxRowBits = 0;
    ClickableLabel *m_ClickableLabelComet = nullptr;
    ClickableLabel *m_ClickableLabelAsteroid = nullptr;
    QCheckBox *m_CheckBoxAsteroid = nullptr;
    QCheckBox *m_CheckBoxComet = nullptr;

    void openFile();
    void downloadFiles();
    void createActions();
    void connectActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void updateJD(double &JD, TimeDirection direction);
    void updateDateTimeEdit(double JD);
};

#endif // MAINWINDOW_H
