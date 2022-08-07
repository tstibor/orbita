#include "mainwindow.h"

static const QStringList RENDER_SETTINGS_STR_FIELDS = {
    "Orbit",
    "Name",
    "Date",
    "Magitude",
    "Distances"
};

static const QStringList RENDER_SETTINGS_TOOLTIP_FIELDS = {
    "Draw orbit",
    "Show designation name",
    "Show date at current position",
    "Show magnitude",
    "Show distance to sun and earth"
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    setWindowTitle(tr("Orbita"));
    setWindowIcon(QIcon(":/res/icons/orbita.png"));
    setUnifiedTitleAndToolBarOnMac(true);

    m_SolarSystem = new SolarSystem();
    m_RenderWindow = new RenderWindow(this, m_SolarSystem);
    m_AsteroidTableDialog = new AsteroidTableDialog(this, m_SolarSystem);
    m_CometTableDialog = new CometTableDialog(this, m_SolarSystem);
    m_MpcToSql = new MpcToSql(this);

    QSurfaceFormat SurfaceFormat;
    SurfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    SurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    m_RenderWindow->setFormat(SurfaceFormat);

    setCentralWidget(m_RenderWindow);

    createActions();
    createStatusBar();
    connectActions();

    readSettings();
}

MainWindow::~MainWindow()
{
    if (m_SolarSystem)
        delete m_SolarSystem;
    if (m_StatusbarWidget)
        delete m_StatusbarWidget;
    if (m_RenderWindow)
        delete m_RenderWindow;
    if (m_AsteroidTableDialog)
	delete m_AsteroidTableDialog;
    if (m_CometTableDialog)
	delete m_CometTableDialog;
    if (m_MpcToSql)
	delete m_MpcToSql;
}

void MainWindow::updateJD(double &JD, TimeDirection direction)
{
    QDateTime dateTime;
    const qreal factor = m_SpinBoxIncrDecrTime->value();

    if (m_ComboBoxIncrDecrTime->currentIndex() == 0) /* Minutes. */
	dateTime = m_DateTimeEdit->dateTime().addSecs(direction == TimeDirection::INCREMENT ? factor * 60 : factor * -60);
    else if (m_ComboBoxIncrDecrTime->currentIndex() == 1) /* Hours. */
	dateTime = m_DateTimeEdit->dateTime().addSecs(direction == TimeDirection::INCREMENT ? factor * 3600 : factor * -3600);
    else if (m_ComboBoxIncrDecrTime->currentIndex() == 2) /* Days. */
	dateTime = m_DateTimeEdit->dateTime().addDays(direction == TimeDirection::INCREMENT ? factor * 1 : factor * -1);
    else if (m_ComboBoxIncrDecrTime->currentIndex() == 3) /* Months. */
	dateTime = m_DateTimeEdit->dateTime().addMonths(direction == TimeDirection::INCREMENT ? factor * 1 : factor * -1);
    else if (m_ComboBoxIncrDecrTime->currentIndex() == 4) /* Years. */
	dateTime = m_DateTimeEdit->dateTime().addYears(direction == TimeDirection::INCREMENT ? factor * 1 : factor * -1);

    JD = DateTimeToJD(dateTime);
}

void MainWindow::updateDateTimeEdit(double JD)
{
    struct ln_date date;

    ln_get_date(m_SolarSystem->JD(), &date);
    m_DateTimeEdit->setDate(QDate(date.years, date.months, date.days));
    m_DateTimeEdit->setTime(QTime(date.hours, date.minutes, date.seconds));
}

void MainWindow::connectActions()
{
    connect(m_SolarSystem, &SolarSystem::planetsPositionChanged, m_StatusbarWidget,
            &StatusbarWidget::updateDate);

    connect(m_AsteroidTableDialog, &AsteroidTableDialog::selectedAsteroid, [&](struct asteroid_t &asteroid) {
	Asteroid a(asteroid, m_SolarSystem->JD());
	m_SolarSystem->addAsteroid(a);
    });
    connect(m_AsteroidTableDialog, &AsteroidTableDialog::displaySelectedAsteroids, [&]() {
	m_SolarSystem->computeAsteroidsOrbit();
	emit m_SolarSystem->asteroidsPositionChanged(m_SolarSystem->JD());
    });

    connect(m_CometTableDialog, &CometTableDialog::selectedComet, [&](struct comet_t &comet) {
	Comet c(comet, m_SolarSystem->JD());
	m_SolarSystem->addComet(c);
    });
    connect(m_CometTableDialog, &CometTableDialog::displaySelectedComets, [&]() {
	m_SolarSystem->computeCometsOrbit();
	emit m_SolarSystem->cometsPositionChanged(m_SolarSystem->JD());
    });

    connect(m_RenderWindow, &RenderWindow::projectionChanged, m_StatusbarWidget,
            &StatusbarWidget::updateProjection);

    connect(m_PushButtonIncrTime, &QPushButton::pressed, m_SolarSystem, [&]() {
	double JD = m_SolarSystem->JD();
	updateJD(JD, TimeDirection::INCREMENT);
	m_SolarSystem->setJD(JD);
	updateDateTimeEdit(JD);
    });

    connect(m_PushButtonDecrTime, &QPushButton::pressed, m_SolarSystem, [&]() {
	double JD = m_SolarSystem->JD();
	updateJD(JD, TimeDirection::DECREMENT);
	m_SolarSystem->setJD(JD);
	updateDateTimeEdit(JD);
    });

    connect(m_CheckBoxAsteroid, &QCheckBox::stateChanged, [&](int state) {
	if (state == Qt::Checked)
	    emit updateRenderSettingAsteroid(m_ComboBoxRowBits);

	m_ComboBoxRenderSettings->setEnabled(m_CheckBoxAsteroid->isChecked()
					     | m_CheckBoxComet->isChecked());
    });
    connect(m_CheckBoxComet, &QCheckBox::stateChanged, [&](int state) {
	if (state == Qt::Checked)
	    emit updateRenderSettingComet(m_ComboBoxRowBits);

	m_ComboBoxRenderSettings->setEnabled(m_CheckBoxAsteroid->isChecked()
					     | m_CheckBoxComet->isChecked());
    });

    connect(m_SliderRadiusAsteroids, &QSlider::valueChanged, m_RenderWindow, &RenderWindow::setRadiusFactorAsteroids);
    connect(m_ComboBoxRenderSettings->model(), &QAbstractItemModel::dataChanged,
	    [&](const QModelIndex &ModelIndex) {
		const quint8 row = ModelIndex.row();

		if (m_QListComboBoxRows.contains(row)) {
		    m_QListComboBoxRows.removeOne(row);
		    m_ComboBoxRowBits &= ~(row == 0 ? (row + 1) : (1 << row));
		} else {
		    m_QListComboBoxRows.append(row);
		    m_ComboBoxRowBits |= (row == 0 ? (row + 1) : (1 << row));
		}
		if (m_CheckBoxAsteroid->isChecked())
		    emit updateRenderSettingAsteroid(m_ComboBoxRowBits);
		if (m_CheckBoxComet->isChecked())
		    emit updateRenderSettingComet(m_ComboBoxRowBits);
	    });

    connect(m_SliderRadiusPlanetsSun, &QSlider::valueChanged, m_RenderWindow, &RenderWindow::setRadiusFactorPlanetsSun);
    connect(m_CheckBoxRadiusPlanetsSun, &QCheckBox::stateChanged, m_RenderWindow, &RenderWindow::setRadiusSamePlanetsSun);
    connect(this, &MainWindow::updateRenderSettingAsteroid, m_RenderWindow, &RenderWindow::renderSettingAsteroid);
    connect(this, &MainWindow::updateRenderSettingComet, m_RenderWindow, &RenderWindow::renderSettingComet);

    connect(m_RenderWindow, &RenderWindow::resetViewChanged, this, [&] {
        m_SliderRadiusPlanetsSun->setValue(50);
	m_SliderRadiusAsteroids->setValue(50);
	m_RenderWindow->setRadiusFactorPlanetsSun(m_SliderRadiusPlanetsSun->value());
	m_RenderWindow->setRadiusFactorAsteroids(m_SliderRadiusAsteroids->value());
	m_CheckBoxRadiusPlanetsSun->setCheckState(Qt::Checked);

	for (auto r = 0; r < RENDER_SETTINGS_STR_FIELDS.count(); ++r) {
	    QStandardItem *item = m_StandardItemModel->item(r);
	    item->setData(Qt::Unchecked, Qt::CheckStateRole);
	}
    });

    connect(m_DateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [&](const QDateTime &DateTime) {
	double JD = DateTimeToJD(DateTime);
	if (JD != m_SolarSystem->JD()) {
	    m_SolarSystem->setJD(JD);
	    updateDateTimeEdit(JD);
	}
    });

    connect(m_ClickableLabelAsteroid, &ClickableLabel::clicked, [&]() {
	m_AsteroidTableDialog->showQueryInTable();
    });

    connect(m_ClickableLabelComet, &ClickableLabel::clicked, [&]() {
	m_CometTableDialog->showQueryInTable();
    });
}

void MainWindow::createActions()
{
    QToolBar *toolBar = addToolBar(tr("Toolbar"));

    /* Menu File. */
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    /* Menu File->Open. */
    const QIcon openIcon = QIcon::fromTheme("document-open");
    QAction *openAct = new QAction(openIcon, tr("&Open MPC file"), this);
    openAct->setToolTip(tr("Open MPC file"));
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAct);
    toolBar->addAction(openAct);

    /* Menu File->Download. */
    const QIcon downloadIcon = QIcon::fromTheme("emblem-downloads");
    QAction *downloadAct = new QAction(downloadIcon, tr("&Download MPC files"), this);
    downloadAct->setToolTip(tr("Download MPC files"));
    connect(downloadAct, &QAction::triggered, this, &MainWindow::downloadFiles);

    fileMenu->addAction(downloadAct);
    toolBar->addAction(downloadAct);

    /* Tables. */
    QMenu *tableMenu = menuBar()->addMenu(tr("&Table"));

    QAction *tableAsteroid = tableMenu->addAction(tr("&Asteroids"), m_AsteroidTableDialog, [&] {
	m_AsteroidTableDialog->showQueryInTable();
    });
    tableAsteroid->setToolTip(tr("Show asteroid table"));

    QAction *tableComet = tableMenu->addAction(tr("&Comets"), m_CometTableDialog, [&] {
	m_CometTableDialog->showQueryInTable();
    });
    tableComet->setToolTip(tr("Show comet table"));

    /* Menu View. */
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    QAction *viewResetAct = viewMenu->addAction(tr("&Reset"), m_RenderWindow, &RenderWindow::resetView);
    viewResetAct->setToolTip(tr("Reset view"));
    viewResetAct->setShortcut(Qt::Key_R);

    QAction *viewTopAct = viewMenu->addAction(tr("&Top"), m_RenderWindow, &RenderWindow::topView);
    viewTopAct->setToolTip(tr("View from top"));
    viewTopAct->setShortcut(Qt::Key_T);

    QAction *viewProjectionAct = viewMenu->addAction(tr("&Switch projection"), m_RenderWindow, &RenderWindow::switchProjection);
    viewProjectionAct->setToolTip(tr("Switch between orthogrphic and perspective projection"));
    viewProjectionAct->setShortcut(Qt::Key_P);

    QAction *viewZoomInAct = viewMenu->addAction(tr("&Zoom in"), [&] {
	m_RenderWindow->setZTranslation(0.1);
    });
    viewZoomInAct->setShortcut(Qt::Key_Plus);

    QAction *viewZoomOutAct = viewMenu->addAction(tr("&Zoom out"), [&] {
	m_RenderWindow->setZTranslation(-0.1);
    });
    viewZoomOutAct->setShortcut(Qt::Key_Minus);

    QAction *viewRotateXPosAct = viewMenu->addAction(tr("&Rotate around positive X-axis"), [&] {
	m_RenderWindow->setXRotation(m_RenderWindow->xRot() + 8);
    });
    viewRotateXPosAct->setShortcut(Qt::Key_X);

    QAction *viewRotateXNegAct = viewMenu->addAction(tr("&Rotate around negative X-axis"), [&] {
	m_RenderWindow->setXRotation(m_RenderWindow->xRot() - 8);
    });
    viewRotateXNegAct->setShortcut(Qt::Key_X | Qt::CTRL);

    QAction *viewRotateYPosAct = viewMenu->addAction(tr("&Rotate around positive Y-axis"), [&] {
	m_RenderWindow->setYRotation(m_RenderWindow->yRot() + 8);
    });
    viewRotateYPosAct->setShortcut(Qt::Key_Y);

    QAction *viewRotateYNegAct = viewMenu->addAction(tr("&Rotate around negative Y-axis"), [&] {
	m_RenderWindow->setYRotation(m_RenderWindow->yRot() - 8);
    });
    viewRotateYNegAct->setShortcut(Qt::Key_Y | Qt::CTRL);

    QAction *viewRotateZPosAct = viewMenu->addAction(tr("&Rotate around positive Z-axis"), [&] {
	m_RenderWindow->setZRotation(m_RenderWindow->zRot() + 8);
    });
    viewRotateZPosAct->setShortcut(Qt::Key_Z);

    QAction *viewRotateZNegAct = viewMenu->addAction(tr("&Rotate around negative Z-axis"), [&] {
	m_RenderWindow->setZRotation(m_RenderWindow->zRot() - 8);
    });
    viewRotateZNegAct->setShortcut(Qt::Key_Z | Qt::CTRL);

    QAction *viewIncrementTimeAct = viewMenu->addAction(tr("&Increment time step"), [&] {
	emit m_PushButtonIncrTime->pressed();
    });
    viewIncrementTimeAct->setShortcut(Qt::Key_Right);

    QAction *viewDecrementTimeAct = viewMenu->addAction(tr("&Decrement time step"), [&] {
	emit m_PushButtonDecrTime->pressed();
    });
    viewDecrementTimeAct->setShortcut(Qt::Key_Left);

    /* Menu Program. */
    QMenu *programMenu = menuBar()->addMenu(tr("&Program"));

    const QIcon aboutIcon = QIcon::fromTheme("help-about");
    QAction *aboutAct = programMenu->addAction(aboutIcon, tr("&About"), this, &MainWindow::about);

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = programMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    programMenu->addAction(exitAct);

    m_DateTimeEdit = new QDateTimeEdit(this);
    m_DateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    m_DateTimeEdit->setCalendarPopup(true);
    updateDateTimeEdit(m_SolarSystem->JD());

    const QIcon incrIcon = QIcon::fromTheme("go-next");
    m_PushButtonIncrTime = new QPushButton(this);
    m_PushButtonIncrTime->setIcon(incrIcon);
    m_PushButtonIncrTime->setToolTip(tr("Increment time"));

    const QIcon decrIcon = QIcon::fromTheme("go-previous");
    m_PushButtonDecrTime = new QPushButton(this);
    m_PushButtonDecrTime->setIcon(decrIcon);
    m_PushButtonDecrTime->setToolTip(tr("Decrement time"));

    m_SpinBoxIncrDecrTime = new QSpinBox(this);
    m_SpinBoxIncrDecrTime->setMinimum(1);
    m_SpinBoxIncrDecrTime->setMaximum(100);
    m_SpinBoxIncrDecrTime->setToolTip(tr("Time steps"));
    m_SpinBoxIncrDecrTime->setFocusPolicy(Qt::NoFocus);

    m_ComboBoxIncrDecrTime = new QComboBox(this);
    m_ComboBoxIncrDecrTime->addItems(QString("Minute;Hour;Day;Month;Year").split(";"));
    m_ComboBoxIncrDecrTime->setCurrentIndex(2);
    m_ComboBoxIncrDecrTime->setToolTip(tr("Time units"));
    m_ComboBoxIncrDecrTime->setFocusPolicy(Qt::NoFocus);

    m_SliderRadiusAsteroids = new QSlider(this);
    m_SliderRadiusAsteroids->setOrientation(Qt::Horizontal);
    m_SliderRadiusAsteroids->setMinimum(1);
    m_SliderRadiusAsteroids->setMaximum(100);
    m_SliderRadiusAsteroids->setValue(50);
    m_RenderWindow->setRadiusFactorAsteroids(m_SliderRadiusAsteroids->value());
    m_SliderRadiusAsteroids->setMaximumWidth(100);
    m_SliderRadiusAsteroids->setToolTip(tr("Increase/decrease radius of asteroids and comets"));
    m_SliderRadiusAsteroids->setFocusPolicy(Qt::NoFocus);

    m_LabelPlanet = new QLabel(this);
    m_LabelPlanet->setPixmap(QPixmap(":/res/icons/planets.png"));

    m_CheckBoxRadiusPlanetsSun = new QCheckBox(this);
    m_CheckBoxRadiusPlanetsSun->setText(tr("Radius"));
    m_CheckBoxRadiusPlanetsSun->setCheckState(Qt::Checked);
    m_CheckBoxRadiusPlanetsSun->setToolTip(tr("Equal radius of planets and sun"));

    m_SliderRadiusPlanetsSun = new QSlider(this);
    m_SliderRadiusPlanetsSun->setOrientation(Qt::Horizontal);
    m_SliderRadiusPlanetsSun->setMinimum(1);
    m_SliderRadiusPlanetsSun->setMaximum(100);
    m_SliderRadiusPlanetsSun->setValue(50);
    m_RenderWindow->setRadiusFactorPlanetsSun(m_SliderRadiusPlanetsSun->value());
    m_SliderRadiusPlanetsSun->setMaximumWidth(100);
    m_SliderRadiusPlanetsSun->setToolTip(tr("Increase/decrease radius of planets and sun"));
    m_SliderRadiusPlanetsSun->setFocusPolicy(Qt::NoFocus);

    m_ClickableLabelAsteroid = new ClickableLabel(this);
    m_ClickableLabelAsteroid->setPixmap(QPixmap(":/res/icons/asteroid.png"));
    m_ClickableLabelAsteroid->setToolTip(tableAsteroid->toolTip());

    m_ComboBoxRenderSettings = new QComboBox(this);
    m_ComboBoxRenderSettings->setFocusPolicy(Qt::NoFocus);
    m_StandardItemModel = new QStandardItemModel(this);

    for (qsizetype i = 0; i < RENDER_SETTINGS_STR_FIELDS.count(); ++i) {
	QStandardItem *item = new QStandardItem(RENDER_SETTINGS_STR_FIELDS[i]);
	item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
	item->setToolTip(RENDER_SETTINGS_TOOLTIP_FIELDS[i]);
	m_StandardItemModel->setItem(i, 0, item);
    }

    m_ComboBoxRenderSettings->setModel(m_StandardItemModel);
    m_ComboBoxRenderSettings->setMinimumContentsLength(10);
    m_ComboBoxRenderSettings->setDisabled(true);

    m_ClickableLabelComet = new ClickableLabel(this);
    m_ClickableLabelComet->setPixmap(QPixmap(":/res/icons/comet.png"));
    m_ClickableLabelComet->setToolTip(tableComet->toolTip());

    m_CheckBoxAsteroid = new QCheckBox(this);
    m_CheckBoxAsteroid->setToolTip(tr("Set asteroid render settings"));
    m_CheckBoxComet = new QCheckBox(this);
    m_CheckBoxComet->setToolTip(tr("Set comet render settings"));

    QWidget* widgetSpacer = new QWidget(this);
    widgetSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(widgetSpacer);

    toolBar->addWidget(m_ClickableLabelComet);
    toolBar->addWidget(m_CheckBoxComet);

    toolBar->addWidget(m_ClickableLabelAsteroid);
    toolBar->addWidget(m_CheckBoxAsteroid);
    toolBar->addWidget(m_ComboBoxRenderSettings);
    toolBar->addWidget(m_SliderRadiusAsteroids);

    toolBar->addWidget(m_LabelPlanet);
    toolBar->addWidget(m_CheckBoxRadiusPlanetsSun);
    toolBar->addWidget(m_SliderRadiusPlanetsSun);

    toolBar->addWidget(m_DateTimeEdit);
    toolBar->addWidget(m_PushButtonDecrTime);
    toolBar->addWidget(m_PushButtonIncrTime);
    toolBar->addWidget(m_SpinBoxIncrDecrTime);
    toolBar->addWidget(m_ComboBoxIncrDecrTime);
}

void MainWindow::createStatusBar()
{
    m_StatusbarWidget = new StatusbarWidget(this);
    statusBar()->addWidget(m_StatusbarWidget);
    m_StatusbarWidget->updateDate(m_SolarSystem->JD());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    Logger::instance()->close();
    event->accept();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Orbita"),
		       "<b>Orbita</b> version: " + QString(orbita_VERSION) + " build: " + QString(orbita_BUILD_TS) +
		       "<br><br>" +
		       "Copyright (C) 2022 Thomas Stibor thomas@stibor.net<br><br>" +
		       "This program is free software; you can redistribute it and/or " +
		       "modify it under the terms of the GNU General Public License " +
		       "as published by the Free Software Foundation; either version 3 " +
		       "of the License, or (at your option) any later version. " +
		       "This program is distributed in the hope that it will be useful, " +
		       "but WITHOUT ANY WARRANTY; without even the implied warranty of " +
		       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the " +
		       "GNU General Public License for more details.");
}

void MainWindow::readSettings()
{
    QMap<QString, QVariant> settings;
    bool success;

    success = Sqlite::readSettings(settings);
    const QByteArray geometry = settings["geometry"].toByteArray();

    if (success && geometry.isEmpty()) {
        qWarning() << tr("window geometry cannot be restored from sqlite database")
                   << Settings::FilenameDb();
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() * 0.85, availableGeometry.height() * 0.85);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        qDebug() << tr("window geometry restored from sqlite database") << Settings::FilenameDb();
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    bool success;

    success = Sqlite::writeSetting("geometry", saveGeometry());
    if (success)
        qDebug() << tr("window geometry stored in sqlite database");
    else
        qWarning() << tr("window geometry failed to store in sqlite database");
}

void MainWindow::openFile()
{
    QFileDialog fileDialog(this, tr("Open MPC file"), QString(),
                           "MPCORB (*.DAT *.gz);;CometEls (*.txt);;All files (*.*)");
    QStringList fileNames;

    if (QDialog::Accepted == fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if (!fileNames.empty() && !fileNames.at(0).isEmpty()) {

	    const QString &filename = QFileInfo(fileNames.at(0)).fileName().toUpper();
	    if (filename == "MPCORB.DAT")
		m_MpcToSql->processFile(fileNames.at(0), OrbType::ASTEROID);
	    else if (filename == "COMETELS.TXT")
		m_MpcToSql->processFile(fileNames.at(0), OrbType::COMET);
	    else /* Guess it is an asteroid file. */
		m_MpcToSql->processFile(fileNames.at(0), OrbType::ASTEROID);
	}
    }
}

void MainWindow::downloadFiles()
{
    DownloadDialog downloadDialog(this);

    connect(&downloadDialog, &DownloadDialog::fileDownloaded, [&](const QString &filename)
    {
	if (filename.toUpper() == "MPCORB.DAT")
	    m_MpcToSql->processFile(filename, OrbType::ASTEROID);
	else if (filename.toUpper() == "COMETELS.TXT")
	    m_MpcToSql->processFile(filename, OrbType::COMET);
	else /* Guess it is an asteroid file. */
	    m_MpcToSql->processFile(filename, OrbType::ASTEROID);
    });
    downloadDialog.exec();
}
