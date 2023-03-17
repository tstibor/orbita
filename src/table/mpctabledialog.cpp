#include "mpctabledialog.h"

MpcTableDialog::MpcTableDialog(QWidget *parent, SolarSystem *solarSystem)
    : QDialog(parent), m_SolarSystem(*solarSystem)
{
    setWindowTitle(tr("Minor Planet Center Objects"));

    m_TabWidget = new QTabWidget;
    m_TableViewAsteroids = new MpcTableView(QUERY_ASTEROID, MpcType::ASTEROID);
    m_TableViewComets = new MpcTableView(QUERY_COMET, MpcType::COMET);

    m_TableViewAsteroids->setSizeAdjustPolicy(QTableView::AdjustToContents);
    m_TableViewAsteroids->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_TableViewAsteroids->installEventFilter(this);
    m_TableViewAsteroids->setSortingEnabled(false);

    m_TableViewComets->setSizeAdjustPolicy(QTableView::AdjustToContents);
    m_TableViewComets->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_TableViewComets->installEventFilter(this);
    m_TableViewComets->setSortingEnabled(false);

    m_TabWidget->addTab(m_TableViewAsteroids, tr("Asteroids"));
    m_TabWidget->addTab(m_TableViewComets, tr("Comets"));

    m_TableViewAsteroids->setSizeAdjustPolicy(QTableView::AdjustToContents);
    m_TableViewAsteroids->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_TableViewComets->setSizeAdjustPolicy(QTableView::AdjustToContents);
    m_TableViewComets->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_VBoxLayout = new QVBoxLayout;
    m_HBoxLayoutQueryCmd = new QHBoxLayout;
    m_GridLayoutSettings = new QGridLayout;

    m_PushButtonDisplay = new QPushButton(tr("Display"));
    m_PushButtonClear = new QPushButton(tr("Clear"));
    m_LabelQueryCmd = new QLabel(tr("SQL query"));
    m_LineEdit = new QLineEdit(this);
    m_LineEdit->installEventFilter(this);

    m_LabelDisplayOptions = new QLabel(tr("with options:"));
    m_CheckBoxOrbit = new QCheckBox(tr("Orbit"));
    m_CheckBoxOrbit->setToolTip(tr("Draw orbit"));
    m_CheckBoxName = new QCheckBox(tr("Name"));
    m_CheckBoxName->setToolTip(tr("Show designation name"));
    m_CheckBoxDate = new QCheckBox(tr("Date"));
    m_CheckBoxDate->setToolTip(tr("Show date"));
    m_CheckBoxMag = new QCheckBox(tr("Magnitude"));
    m_CheckBoxMag->setToolTip(tr("Show magnitude for current date"));
    m_CheckBoxDist = new QCheckBox(tr("Distance"));
    m_CheckBoxDist->setToolTip(tr("Show distance to sun and earth for current date"));

    m_GridLayoutSettings->addWidget(m_PushButtonDisplay, 0, 0, 1, 2);
    m_GridLayoutSettings->addWidget(m_LabelDisplayOptions, 0, 2);
    m_GridLayoutSettings->addWidget(m_CheckBoxOrbit, 0, 3);
    m_GridLayoutSettings->addWidget(m_CheckBoxName, 0, 4);
    m_GridLayoutSettings->addWidget(m_CheckBoxDate, 0, 5);
    m_GridLayoutSettings->addWidget(m_CheckBoxMag, 0, 6);
    m_GridLayoutSettings->addWidget(m_CheckBoxDist, 0, 7);
    m_GridLayoutSettings->addLayout(new QHBoxLayout, 0, 8, 1, 8);
    m_GridLayoutSettings->addWidget(m_PushButtonClear, 0, 15, 1, 2);

    m_VBoxLayout->addWidget(m_TabWidget);

    m_HBoxLayoutQueryCmd->addWidget(m_LabelQueryCmd);
    m_HBoxLayoutQueryCmd->addWidget(m_LineEdit);
    m_VBoxLayout->addLayout(m_HBoxLayoutQueryCmd);
    m_VBoxLayout->addLayout(m_GridLayoutSettings);

    setLayout(m_VBoxLayout);

    connectSignalsToSlots();
}

void MpcTableDialog::switchOptions(int state, quint16 optionBit)
{
    MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
    if (!TableView)
	return;

    if (state)
	m_DisplayOptions[TableView->celestialType()] |= optionBit;
    else
	m_DisplayOptions[TableView->celestialType()] &= ~optionBit;

    emit updateDisplayOptions(m_DisplayOptions[TableView->celestialType()], TableView->celestialType());
}

void MpcTableDialog::connectSignalsToSlots()
{
    connect(m_TabWidget, &QTabWidget::currentChanged, [=, this](int index) {
	updateCheckBoxes(index);
	prepareOpen();
    });
    connect(m_PushButtonDisplay, &QPushButton::clicked, [=, this] { renderSelectedAsteroids(); renderSelectedComets(); });
    connect(this, &MpcTableDialog::numberRows, this, &MpcTableDialog::updateWindowTitle);
    connect(m_CheckBoxOrbit, &QCheckBox::stateChanged, [=, this](int state) { switchOptions(state, RENDER_ORBIT); });
    connect(m_CheckBoxName, &QCheckBox::stateChanged, [=, this](int state) { switchOptions(state, RENDER_NAME); });
    connect(m_CheckBoxDate, &QCheckBox::stateChanged, [=, this](int state) { switchOptions(state, RENDER_DATE); });
    connect(m_CheckBoxMag, &QCheckBox::stateChanged, [=, this](int state) { switchOptions(state, RENDER_MAG); });
    connect(m_CheckBoxDist, &QCheckBox::stateChanged, [=, this](int state) { switchOptions(state, RENDER_DIST); });
}

void MpcTableDialog::queryResultsToTableView()
{
    Sqlite connection(Settings::FilenameDb());
    QSqlDatabase db = connection.getDb();

    if (!db.isOpen()) {
	QMessageBox::critical(this, tr("Database error"),
			      tr("Cannot open sqlite database"));
        return;
    }

    MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
    QSqlQueryModel *SqlQueryModel = TableView->attachedSqlQueryModel();

    emit queryStarted();

    /* Query all fields and show the entries in TableView. */
    SqlQueryModel->setQuery(QSqlQuery(m_SqlCmdQuery, db));
    while (SqlQueryModel->canFetchMore())
	SqlQueryModel->fetchMore();

    /* Attach the SQL fetched rows to the table view. */
    TableView->setModel(SqlQueryModel);
    connect(TableView->selectionModel(), &QItemSelectionModel::selectionChanged,
	    [=, this](const QItemSelection &selected, const QItemSelection &deselected)
	    {
		QItemSelectionModel *selectionModel = TableView->selectionModel();
		if (selectionModel->hasSelection()) {
		    emit numberRows(selectionModel->selectedRows().size(), SqlQueryModel->rowCount());
		}
	    });

    emit numberRows(0, SqlQueryModel->rowCount());
    emit queryFinished();
}

void MpcTableDialog::updateWindowTitle(quint32 numSelectedObjects, quint32 numTotalObjects)
{
    QString type;

    if (m_TabWidget->currentIndex() == 0)
	type = tr("asteroids");
    else if (m_TabWidget->currentIndex() == 1)
	type = tr("comets");
    else
	type = "";

    setWindowTitle(tr("Number of selected")
		   + " "
		   + type
		   + " "
		   + QString::number(numSelectedObjects)
		   + " / "
		   + QString::number(numTotalObjects));
}

void MpcTableDialog::prepareQueryForTable(MpcType type)
{
    if (type == MpcType::ASTEROID) {
	setSqlCmdQuery(QUERY_ASTEROID);
    } else if (type == MpcType::COMET) {
	setSqlCmdQuery(QUERY_COMET);
    }
}

bool MpcTableDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_LineEdit && event->type() == QEvent::KeyPress) {

	MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
	if (TableView != nullptr) {
	    TableView->setSqlQueryCmd(m_LineEdit->text());
	}

        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
	if (keyEvent->key() == Qt::Key_Return) {
	    m_SqlCmdQuery = m_LineEdit->text();
	    queryResultsToTableView();
	}
    }

    return QObject::eventFilter(object, event);
}

void MpcTableDialog::closeEvent(QCloseEvent *event)
{
    hide();
}

void MpcTableDialog::openAndShow()
{
    prepareOpen();
    show();
}

void MpcTableDialog::prepareOpen()
{
    MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
    if (TableView->model() == nullptr) {
	if (m_TabWidget->currentIndex() == 0) {
	    prepareQueryForTable(MpcType::ASTEROID);
	    m_LineEdit->setText(QUERY_ASTEROID);
	} else if (m_TabWidget->currentIndex() == 1) {
	    prepareQueryForTable(MpcType::COMET);
	    m_LineEdit->setText(QUERY_COMET);
	}
	queryResultsToTableView();
    } else {
	m_LineEdit->setText(TableView->SqlQueryCmd());
	QSqlQueryModel *SqlQueryModel = TableView->attachedSqlQueryModel();
	QItemSelectionModel *selectionModel = TableView->selectionModel();
	if (SqlQueryModel && selectionModel && selectionModel->hasSelection()) {
	    emit numberRows(selectionModel->selectedRows().size(), SqlQueryModel->rowCount());
	} else {
	    emit numberRows(0, 0);
	}
    }
}

void MpcTableDialog::renderSelectedAsteroids()
{
    MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
    if (!TableView || TableView->celestialType() != MpcType::ASTEROID) {
	return;
    }

    m_SolarSystem.removeAsteroids();

    QItemSelectionModel *selectionModel = TableView->selectionModel();
    if (selectionModel && selectionModel->hasSelection()) {

	struct asteroid_t asteroid;

	foreach(const QModelIndex &index, selectionModel->selectedRows()) {

	    memset(&asteroid, 0, sizeof(asteroid));

	    strncpy(asteroid.designation_readable, index.siblingAtColumn(0).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.designation_readable));
	    asteroid.eccentricity = index.siblingAtColumn(1).data(Qt::DisplayRole).toDouble();
	    asteroid.semimajor_axis = index.siblingAtColumn(2).data(Qt::DisplayRole).toDouble();
	    asteroid.inclination = index.siblingAtColumn(3).data(Qt::DisplayRole).toDouble();
	    asteroid.ascending_node = index.siblingAtColumn(4).data(Qt::DisplayRole).toDouble();
	    asteroid.perihelion = index.siblingAtColumn(5).data(Qt::DisplayRole).toDouble();
	    asteroid.anomaly_mean = index.siblingAtColumn(6).data(Qt::DisplayRole).toDouble();
	    asteroid.daily_motion = index.siblingAtColumn(7).data(Qt::DisplayRole).toDouble();
	    strncpy(asteroid.epoch, index.siblingAtColumn(8).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.epoch));
	    asteroid.classification_flag = index.siblingAtColumn(9).data(Qt::DisplayRole).toUInt();
	    strncpy(asteroid.designation, index.siblingAtColumn(10).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.designation));
	    asteroid.observations = index.siblingAtColumn(11).data(Qt::DisplayRole).toUInt();
 	    strncpy(asteroid.last_observation, index.siblingAtColumn(12).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(asteroid.last_observation));
	    asteroid.H = index.siblingAtColumn(13).data(Qt::DisplayRole).toDouble();
	    asteroid.G = index.siblingAtColumn(14).data(Qt::DisplayRole).toDouble();

	    emit selectedAsteroid(asteroid);
	}
	emit displaySelection(MpcType::ASTEROID);
    }
}

void MpcTableDialog::renderSelectedComets()
{
    MpcTableView *TableView = dynamic_cast<MpcTableView *>(m_TabWidget->currentWidget());
    if (!TableView || TableView->celestialType() != MpcType::COMET) {
	return;
    }

    m_SolarSystem.removeComets();

    QItemSelectionModel *selectionModel = TableView->selectionModel();
    if (selectionModel && selectionModel->hasSelection()) {

	struct comet_t comet;

	foreach(const QModelIndex &index, selectionModel->selectedRows()) {

	    memset(&comet, 0, sizeof(comet));

	    strncpy(comet.designation_name, index.siblingAtColumn(0).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(comet.designation_name));
	    comet.eccentricity        = index.siblingAtColumn(1).data(Qt::DisplayRole).toDouble();
	    comet.inclination         = index.siblingAtColumn(2).data(Qt::DisplayRole).toDouble();
	    comet.ascending_node      = index.siblingAtColumn(3).data(Qt::DisplayRole).toDouble();
	    comet.perihelion          = index.siblingAtColumn(4).data(Qt::DisplayRole).toDouble();
	    comet.perihelion_year     = index.siblingAtColumn(5).data(Qt::DisplayRole).toUInt();
	    comet.perihelion_month    = index.siblingAtColumn(6).data(Qt::DisplayRole).toUInt();
	    comet.perihelion_day      = index.siblingAtColumn(7).data(Qt::DisplayRole).toDouble();
	    comet.perihelion_distance = index.siblingAtColumn(8).data(Qt::DisplayRole).toDouble();
	    strncpy(comet.orbit_type, index.siblingAtColumn(9).data(Qt::DisplayRole)
		    .toString().toStdString().c_str(), sizeof(comet.orbit_type));
	    comet.H = index.siblingAtColumn(10).data(Qt::DisplayRole).toDouble();
	    comet.G = index.siblingAtColumn(11).data(Qt::DisplayRole).toDouble();

	    emit selectedComet(comet);
	}
	emit displaySelection(MpcType::COMET);
    }
}

void MpcTableDialog::updateCheckBoxes(int index)
{
    m_CheckBoxOrbit->setChecked(m_DisplayOptions[index] & RENDER_ORBIT);
    m_CheckBoxName->setChecked(m_DisplayOptions[index] & RENDER_NAME);
    m_CheckBoxDate->setChecked(m_DisplayOptions[index] & RENDER_DATE);
    m_CheckBoxMag->setChecked(m_DisplayOptions[index] & RENDER_MAG);
    m_CheckBoxDist->setChecked(m_DisplayOptions[index] & RENDER_DIST);
}
