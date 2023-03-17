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
#include "mpctabledialog.h"
#include "solarsystem.h"
#include "renderwindow.h"
#include "statusbarwidget.h"
#include "sqlite.h"
#include "mpcparser.h"
#include "mpctosql.h"
#include "progressdialog.h"
#include "downloaddialog.h"
#include "clickablelabel.h"
#include "version.h"

enum class TimeDirection { INCREMENT, DECREMENT };

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
    MpcTableDialog *m_MpcTableDialog = nullptr;
    MpcToSql *m_MpcToSql = nullptr;

    QDateTimeEdit *m_DateTimeEdit = nullptr;
    QPushButton *m_PushButtonIncrTime = nullptr;
    QPushButton *m_PushButtonDecrTime = nullptr;
    QSpinBox *m_SpinBoxIncrDecrTime = nullptr;
    QComboBox *m_ComboBoxIncrDecrTime = nullptr;
    QLabel *m_LabelPlanet = nullptr;
    QCheckBox *m_CheckBoxRadiusPlanetsSun = nullptr;
    QSlider *m_SliderRadiusPlanetsSun = nullptr;

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
